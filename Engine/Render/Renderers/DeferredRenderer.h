#pragma once
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Vulkan/DynamicRendering.h"

class DeferredRenderer
{
public:
	static void Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
private:
	static void RenderDirectionLights(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
	static void RenderPointLights(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
	static void RenderSpotLights(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
};
