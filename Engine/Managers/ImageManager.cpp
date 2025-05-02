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

    imageLoadFutures.emplace(path, std::async(std::launch::async, &ImageTexture::Load, images.at(path), path, generateMipMap));

	return images.at(path);
}

void ImageManager::Update()
{
    //Need the lock, might delete future from futures map while LoadModel inserts future in it.
    std::unique_lock<std::mutex> lock(loadMutex);

    for (auto it = imageLoadFutures.begin(); it != imageLoadFutures.end();) {
        if (it->second.valid() && it->second.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                it->second.get();

                std::string path = it->first;
                imagesToUploadGpu.push_back(images.at(path));

                it = imageLoadFutures.erase(it);
            }
            catch (const std::exception& e) {
                it = imageLoadFutures.erase(it);
            }
        }
        else {
            ++it;
        }
    }

    if (imagesToUploadGpu.size() >= minSubmitBatchSize || (!imagesToUploadGpu.empty() && imageLoadFutures.empty())) 
    {
        imagesToUploadGpuFutures.insert(std::make_unique<std::future<void>>(std::async(std::launch::async, &ImageManager::UploadBatchedImages, this, imagesToUploadGpu)));
        imagesToUploadGpu.clear();
    }

    for (auto it = imagesToUploadGpuFutures.begin(); it != imagesToUploadGpuFutures.end();) {
        auto& future = *it;
        if (future->valid() && future->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            try {
                future->get();
                it = imagesToUploadGpuFutures.erase(it);
            }
            catch (const std::exception& e) {
                it = imagesToUploadGpuFutures.erase(it);
            }
        }
        else {
            ++it;
        }
    }

    for (auto& [path, image] : images)
    {
        if (image->state == LoadState::GpuUploaded)
        {
            vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", images.at(path)->GetImage()->GetImageView(), VK_NULL_HANDLE, images.at(path)->GetImage()->GetDescriptorArrayIndex());
            image->state = LoadState::Ready;
        }   
    }
}

void ImageManager::UploadBatchedImages(std::vector<std::shared_ptr<ImageTexture>> imagesToUploadGpu)
{
    std::vector<VkCommandBufferSubmitInfo> submitInfos;
    submitInfos.reserve(imagesToUploadGpu.size());

    for (auto& image : imagesToUploadGpu)
        submitInfos.push_back(image->GetCommandBufferSubmitInfo());

    Vk::VulkanContext::GetContext()->GetImmediateQueue()->SubmitGraphics(submitInfos);

    for (auto& image : imagesToUploadGpu)
    {
        image->DestoryCommandPoolAndBuffer();
        image->state = LoadState::GpuUploaded;
    }
}

void ImageManager::WaitForImageFuture(const std::string& path)
{
    /*
    std::unique_lock<std::mutex> lock(loadMutex);

    if (imageLoadFutures.find(path) == imageLoadFutures.end() )
        return;

    if (imageLoadFutures.at(path).valid())
    {
        imageLoadFutures.at(path).get();
        images.at(path)->state = LoadState::Ready;
        vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", images.at(path)->GetImage()->GetImageView(), VK_NULL_HANDLE, images.at(path)->GetImage()->GetDescriptorArrayIndex());
    }

    imageLoadFutures.erase(path);
    */
}
