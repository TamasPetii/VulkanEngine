#pragma once
#include "Engine/EngineApi.h"
#include "Engine/Vulkan/Image.h"
#include "Engine/Async/AsyncLoaded.h"
#include "Engine/Utils/BatchUploaded.h"
#include <memory>
#include <string>

class ENGINE_API ImageTexture : public AsyncLoaded, public BatchUploaded
{
public:
	ImageTexture(uint32_t descriptorArrayIndex);
	void Load(const std::string& path, bool generateMipMap);
	auto& GetImage() { return image; }
	auto GetDescriptorArrayIndex() { return descriptorArrayIndex; }
private:
	void CreateVulkanImageWithGli(const std::string& path);
	void CreateVulkanImageWithStb(const std::string& path, bool generateMipMap);
private:
	uint32_t descriptorArrayIndex;
	std::unique_ptr<Vk::Image> image;
};

