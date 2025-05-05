#pragma once
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Vulkan/DynamicRendering.h"

class GeometryRenderer
{
public:
	static void Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
private:
	static void RenderShapesInstanced(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
	static void RenderModelsInstanced(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
};

