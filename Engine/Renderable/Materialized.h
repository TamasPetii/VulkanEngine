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
	virtual void UploadMaterialDataToGpu();
protected:
	std::vector<uint32_t> materialIndices;
	std::vector<MaterialComponent> materials;
	std::shared_ptr<Vk::Buffer> materialBuffer;
};

