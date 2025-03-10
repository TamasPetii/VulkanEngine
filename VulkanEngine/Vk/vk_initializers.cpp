#include "vk_initializers.h"

VkCommandPoolCreateInfo Vkinit::CommandPoolCreateInfo(uint32_t queueIndex, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo commandPoolInfo{};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.queueFamilyIndex = queueIndex;
	commandPoolInfo.flags = flags;
	commandPoolInfo.pNext = nullptr;

    return commandPoolInfo;
}

VkCommandBufferAllocateInfo Vkinit::CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count)
{
	VkCommandBufferAllocateInfo commandBufferInfo{};
	commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferInfo.commandPool = pool;
	commandBufferInfo.commandBufferCount = count;
	commandBufferInfo.pNext = nullptr;
    return commandBufferInfo;
}

VkFenceCreateInfo Vkinit::FenceCreateInfo(VkFenceCreateFlags flags)
{
	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = flags;
	fenceCreateInfo.pNext = nullptr;

	return fenceCreateInfo;
}

VkSemaphoreCreateInfo Vkinit::SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
	VkSemaphoreCreateInfo semaphoreCreateInfo{};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.flags = flags;
	semaphoreCreateInfo.pNext = nullptr;

	return semaphoreCreateInfo;
}

VkCommandBufferBeginInfo Vkinit::CommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo commandBeginInfo{};
	commandBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBeginInfo.flags = flags;
	commandBeginInfo.pNext = nullptr;
	commandBeginInfo.pInheritanceInfo = nullptr;

	return commandBeginInfo;
}

VkImageSubresourceRange Vkinit::ImageSubResourceRange(VkImageAspectFlags aspectMask)
{
	VkImageSubresourceRange imageRange{};
	imageRange.aspectMask = aspectMask;
	imageRange.baseArrayLayer = 0;
	imageRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
	imageRange.baseMipLevel = 0;
	imageRange.levelCount = VK_REMAINING_MIP_LEVELS;

	return imageRange;
}

VkSemaphoreSubmitInfo Vkinit::SemaphoreSubmintInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
	VkSemaphoreSubmitInfo semaphoreSubmitInfo{};
	semaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	semaphoreSubmitInfo.semaphore = semaphore;
	semaphoreSubmitInfo.stageMask = stageMask;
	semaphoreSubmitInfo.deviceIndex = 0;
	semaphoreSubmitInfo.value = 1;
	semaphoreSubmitInfo.pNext = nullptr;

	return semaphoreSubmitInfo;
}

VkCommandBufferSubmitInfo Vkinit::CommandBufferSubmitInfo(VkCommandBuffer commandBuffer)
{
	VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
	commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmitInfo.commandBuffer = commandBuffer;
	commandBufferSubmitInfo.deviceMask = 0;
	commandBufferSubmitInfo.pNext = nullptr;

	return commandBufferSubmitInfo;
}

VkSubmitInfo2 Vkinit::SubmitInfo(VkCommandBufferSubmitInfo* commandSubmitInfo, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo)
{
	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;

	submitInfo.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
	submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfo;

	submitInfo.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
	submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfo;

	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = commandSubmitInfo;

	return submitInfo;
}

VkImageCreateInfo Vkinit::ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.extent = extent;
	imageCreateInfo.format = format;
	imageCreateInfo.usage = usageFlags;
	imageCreateInfo.pNext = nullptr;

	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	return imageCreateInfo;
}

VkImageViewCreateInfo Vkinit::ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.pNext = nullptr;

	imageViewCreateInfo.format = format;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;

	return imageViewCreateInfo;
}

VkRenderingAttachmentInfo Vkinit::AttachmentInfo(VkImageView imageView, VkClearValue* clearValue, VkImageLayout layoyut)
{
	VkRenderingAttachmentInfo attachmentInfo{};
	attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	attachmentInfo.pNext = nullptr;
	attachmentInfo.imageView = imageView;
	attachmentInfo.imageLayout = layoyut;
	attachmentInfo.loadOp = clearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	if (clearValue)
		attachmentInfo.clearValue = *clearValue;

	return attachmentInfo;
}

VkRenderingInfo Vkinit::RenderingInfo(VkExtent2D extent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment)
{
	VkRenderingInfoKHR renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	renderInfo.pNext = nullptr;
	renderInfo.renderArea.offset = { 0, 0 };
	renderInfo.renderArea.extent = extent;
	renderInfo.layerCount = 1;
	renderInfo.viewMask = 0;
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = colorAttachment;
	renderInfo.pDepthAttachment = depthAttachment;
	renderInfo.pStencilAttachment = nullptr;  // If needed, you can pass this too

	return renderInfo;
}
