#pragma once
#include "Engine/Render/RenderContext.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Vulkan/Buffer.h"

class DeferredRenderer
{
public:
	static void Render(VkCommandBuffer commandBuffer);
};

