#include "ImageManager.h"

ImageManager::ImageManager(std::shared_ptr<VulkanManager> vulkanManager) : 
    ArrayIndexedManager(),
	vulkanManager(vulkanManager)
{
}

ImageManager::~ImageManager()
{
	images.clear();
}

std::shared_ptr<ImageTexture> ImageManager::GetImage(const std::string& path)
{
    std::lock_guard<std::mutex> lock(asyncMutex);

	if (images.find(path) == images.end())
		return nullptr;

	return images.at(path);
}

std::shared_ptr<ImageTexture> ImageManager::LoadImage(const std::string& path, bool generateMipMap)
{
    std::lock_guard<std::mutex> lock(asyncMutex);

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
    std::lock_guard<std::mutex> lock(asyncMutex);

    auto completedFutures = AsyncManager::CompleteFinishedFutures();

    if (imagesToUploadGpu.size() >= minSubmitBatchSize || (!imagesToUploadGpu.empty() && imageLoadFutures.empty())) 
    {
        std::cout << "Images Uploaded with batch size: " << imagesToUploadGpu.size() << "\n";
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
