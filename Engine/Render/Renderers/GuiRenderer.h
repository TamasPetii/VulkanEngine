#pragma once
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Vulkan/DynamicRendering.h"

class GuiRenderer
{
public:
	static void Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, uint32_t imageIndex, const std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)>& guiRenderFunction);
};

