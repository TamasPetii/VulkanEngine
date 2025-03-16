#pragma once
#include <vulkan/vulkan.h>

namespace Vk
{
	class DynamicRendering
	{
	public:
		static VkRenderingAttachmentInfo BuildRenderingAttachmentInfo(VkImageView imageView, VkImageLayout imageLayout, VkClearValue* clearValue);
		static VkRenderingInfo BuildRenderingInfo(VkExtent2D renderArea, VkRenderingAttachmentInfo* colorAttachments, VkRenderingAttachmentInfo* depthAttachment);
	};
}


