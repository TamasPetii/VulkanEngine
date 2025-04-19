#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

#include "Engine/Vulkan/Image.h"
#include "Engine/Managers/VulkanManager.h"

class ENGINE_API ImageManager
{
public:
	ImageManager(std::shared_ptr<VulkanManager> vulkanManager);
	~ImageManager();
	
	std::shared_ptr<Vk::Image> GetImage(const std::string& path);
	std::shared_ptr<Vk::Image> LoadImage(const std::string& path, bool useMipMap = true);
private:
	void Initialize();
	void Cleanup();
	uint32_t GetAvailableIndex();

	std::shared_ptr<Vk::Image> CreateVulkanImageWithGli(const std::string& path);
	std::shared_ptr<Vk::Image> CreateVulkanImageWithStb(const std::string& path, bool generateMipMap);
private:
	uint32_t counter = 0;
	std::set<uint32_t> availableIndices;
	std::shared_ptr<VulkanManager> vulkanManager = nullptr;
	std::unordered_map<std::string, std::shared_ptr<Vk::Image>> images;
};

