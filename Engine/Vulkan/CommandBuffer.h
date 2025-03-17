#pragma once
#include "VulkanContext.h"
#include <string>
#include <vector>
#include <span>

namespace Vk
{
	class ENGINE_API CommandBuffer
	{
	public:
		static VkCommandBufferSubmitInfo BuildSubmitInfo(VkCommandBuffer commandBuffer);
		static VkCommandBuffer BeginSingleTimeCommandBuffer(VkCommandPool commandPool);
		static void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue);
	};
}
