#pragma once
#include "Engine/Render/RenderContext.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Vulkan/Buffer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GeometryRenderer
{
public:
	static void Render(VkCommandBuffer commandBuffer, std::shared_ptr<Vk::Buffer> vertexBuffer, std::shared_ptr<Vk::Buffer> indexBuffer, uint32_t indexCount);
};

