#pragma once
#include "../EngineApi.h"
#include <vulkan/vulkan.h>

namespace Vk
{
	class ENGINE_API DynamicRendering
	{
	public:
		static VkRenderingAttachmentInfo BuildRenderingAttachmentInfo(VkImageView imageView, VkImageLayout imageLayout, VkClearValue* clearValue);
		static VkRenderingInfo BuildRenderingInfo(VkExtent2D renderArea, VkRenderingAttachmentInfo* colorAttachments, VkRenderingAttachmentInfo* depthAttachment);
	};
}


