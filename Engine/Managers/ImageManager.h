#pragma once
#include <memory>
#include <set>
#include <string>
#include <atomic>
#include <future>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "Engine/Utils/ImageTexture.h"
#include "Engine/Managers/VulkanManager.h"

class ENGINE_API ImageManager
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
	uint32_t GetAvailableIndex();
private:
	std::mutex loadMutex;
	std::mutex availableIndexMutex;
	std::atomic<uint32_t> counter = 0;
	std::set<uint32_t> availableIndices;
	std::shared_ptr<VulkanManager> vulkanManager = nullptr;
	std::unordered_map<std::string, std::shared_ptr<ImageTexture>> images;
	std::unordered_map<std::string, std::future<void>> futures;
};

