#pragma once
#include "../EngineApi.h"
#include <vulkan/vulkan.h>
#include <span>

namespace Vk
{
	class ENGINE_API DynamicRendering
	{
	public:
		static VkRenderingAttachmentInfo BuildRenderingAttachmentInfo(VkImageView imageView, VkImageLayout imageLayout, VkClearValue* clearValue);
		static VkRenderingInfo BuildRenderingInfo(VkExtent2D renderArea, std::span<VkRenderingAttachmentInfo> colorAttachments, VkRenderingAttachmentInfo* depthAttachment);
	};
}


