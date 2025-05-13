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

    log << std::format("[Image Thread Started] : {}", path) << "\n";

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

    for (const auto& path : completedFutures)
    {
        auto image = images.at(path);
        if (image->state == LoadState::CpuLoaded)
        {
            log << std::format("[Image Thread Finished] : {}", path) << "\n";
            batch.push_back(image.get());
        }
    }
    
    AsyncBatchedGpuUploadedManager::TryToSubmitBatch(futures.empty());

    auto completedBatchFutures = AsyncBatchedGpuUploadedManager::CompleteFinishedFutures();

    if(!completedBatchFutures.empty())
        log << std::format("[Image Batch Upload Thread Finished] : Batch size = {}", completedBatchFutures.size()) << "\n";

    for (auto image : completedBatchFutures)
    {
        if (image && image->state == LoadState::GpuUploaded)
        {
            vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", image->GetImage()->GetImageView(), VK_NULL_HANDLE, image->GetImage()->GetDescriptorArrayIndex());
            image->state = LoadState::Ready;
        }   
    }
}
