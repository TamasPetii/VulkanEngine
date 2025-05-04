#pragma once
#include <memory>
#include <set>
#include <string>
#include <atomic>
#include <future>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include "Engine/Utils/ImageTexture.h"
#include "Engine/Managers/VulkanManager.h"
#include "DescriptorArrayIndexed.h"

class ENGINE_API ImageManager : public DescriptorArrayIndexed
{
public:
	ImageManager(std::shared_ptr<VulkanManager> vulkanManager);
	~ImageManager();

	ImageManager(const ImageManager&) = delete;
	ImageManager& operator=(const ImageManager&) = delete;

	void Update();
	void WaitForImageFuture(const std::string& path);
	std::shared_ptr<ImageTexture> GetImage(const std::string& path);
	std::shared_ptr<ImageTexture> LoadImage(const std::string& path, bool generateMipMap = true);
private:
	void UploadBatchedImages(std::vector<std::shared_ptr<ImageTexture>> images);
private:
	std::mutex loadMutex;
	std::shared_ptr<VulkanManager> vulkanManager = nullptr;
	std::unordered_map<std::string, std::shared_ptr<ImageTexture>> images;
	std::unordered_map<std::string, std::future<void>> imageLoadFutures;

	constexpr static inline uint32_t minSubmitBatchSize = 16;
	std::vector<std::shared_ptr<ImageTexture>> imagesToUploadGpu;
	std::unordered_set<std::unique_ptr<std::future<void>>> imagesToUploadGpuFutures;
};

