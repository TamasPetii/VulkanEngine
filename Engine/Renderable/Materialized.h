#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Engine/Config.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Components/MaterialComponent.h"

class ENGINE_API Materialized
{
public:
	std::shared_ptr<Vk::Buffer> GetMaterialBuffer();
	std::shared_ptr<Vk::Buffer> GetMaterialIndexBuffer();
	virtual void UploadMaterialDataToGpu();
protected:
	std::vector<uint32_t> materialIndices;
	std::vector<MaterialComponent> materials;
	std::shared_ptr<Vk::Buffer> materialBuffer;
	std::shared_ptr<Vk::Buffer> materialIndexBuffer; // Maches mesh count for models
};

