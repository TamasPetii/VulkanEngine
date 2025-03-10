#pragma once
#include "vulkan/vulkan.h"

class Vkinit
{
public:
	static VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queueIndex, VkCommandPoolCreateFlags flags);
	static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count);
	static VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags = NULL);
	static VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags = NULL);
	static VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags);
	static VkImageSubresourceRange ImageSubResourceRange(VkImageAspectFlags aspectMask);

	static VkSemaphoreSubmitInfo SemaphoreSubmintInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
	static VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer commandBuffer);
	static VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* commandSubmitInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);

	static VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
	static VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
	
	static VkRenderingAttachmentInfo AttachmentInfo(VkImageView imageView, VkClearValue* clearValue, VkImageLayout layoyut = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	static VkRenderingInfo RenderingInfo(VkExtent2D extent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment);
};

