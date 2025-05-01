#include "ImageManager.h"

ImageManager::ImageManager(std::shared_ptr<VulkanManager> vulkanManager) :
	vulkanManager(vulkanManager)
{
}

ImageManager::~ImageManager()
{
	images.clear();
}

uint32_t ImageManager::GetAvailableIndex()
{
	std::unique_lock<std::mutex> lock(availableIndexMutex);

	if (availableIndices.empty())
		return counter++;

	uint32_t index = *availableIndices.begin();
	availableIndices.erase(index);
	return index;
}

std::shared_ptr<ImageTexture> ImageManager::GetImage(const std::string& path)
{
	if (images.find(path) == images.end())
		return nullptr;

	return images.at(path);
}

std::shared_ptr<ImageTexture> ImageManager::LoadImage(const std::string& path, bool generateMipMap)
{
	std::unique_lock<std::mutex> lock(loadMutex);

	if (images.find(path) != images.end())
		return images.at(path);

	std::shared_ptr<ImageTexture> imageTexture = std::make_shared<ImageTexture>(GetAvailableIndex());
	images[path] = imageTexture;

	futures.emplace(path, std::async(std::launch::async, &ImageTexture::Load, images.at(path), path, generateMipMap));

	return images.at(path);
}

void ImageManager::Update()
{
    //Need the lock, might delete future from futures map while LoadModel inserts future in it.
    std::unique_lock<std::mutex> lock(loadMutex);

    for (auto it = futures.begin(); it != futures.end();) {
        if (it->second.valid() && it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                it->second.get();
                it = futures.erase(it);
                std::cout << "Async texture loading thread finished successfuly" << "\n";
            }
            catch (const std::exception& e) {
                std::cout << "Async texture loading thread error: " << e.what() << std::endl;
            }
        }
        else {
            ++it;
        }
    }

    for (auto& [path, image] : images)
    {
        if (futures.find(path) == futures.end() && image->state == LoadState::Loaded)
        {
            image->state = LoadState::Ready;
            vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", images.at(path)->GetImage()->GetImageView(), VK_NULL_HANDLE, images.at(path)->GetImage()->GetDescriptorArrayIndex());
        }   
    }
}

void ImageManager::WaitForImageFuture(const std::string& path)
{
    std::unique_lock<std::mutex> lock(loadMutex);

    if (futures.find(path) == futures.end() )
        return;

    if (futures.at(path).valid())
    {
        futures.at(path).get();
        images.at(path)->state = LoadState::Ready;
        vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", images.at(path)->GetImage()->GetImageView(), VK_NULL_HANDLE, images.at(path)->GetImage()->GetDescriptorArrayIndex());
        std::cout << "WaitForImageFuture finished, image loaded: " << path << "\n";
    }

    futures.erase(path);
}
