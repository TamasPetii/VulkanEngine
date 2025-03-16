#include "DynamicRendering.h"

VkRenderingAttachmentInfo Vk::DynamicRendering::BuildRenderingAttachmentInfo(VkImageView imageView, VkImageLayout imageLayout, VkClearValue* clearValue)
{
	VkRenderingAttachmentInfo attachmentInfo{};
	attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	attachmentInfo.pNext = nullptr;
	attachmentInfo.imageView = imageView;
	attachmentInfo.imageLayout = imageLayout;
	attachmentInfo.loadOp = clearValue ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	if (clearValue)
		attachmentInfo.clearValue = *clearValue;

	return attachmentInfo;
}

VkRenderingInfo Vk::DynamicRendering::BuildRenderingInfo(VkExtent2D renderArea, VkRenderingAttachmentInfo* colorAttachments, VkRenderingAttachmentInfo* depthAttachment)
{
	VkRenderingInfoKHR renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
	renderInfo.pNext = nullptr;
	renderInfo.renderArea.offset = { 0, 0 };
	renderInfo.renderArea.extent = renderArea;
	renderInfo.layerCount = 1;
	renderInfo.viewMask = 0;
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = colorAttachments;
	renderInfo.pDepthAttachment = depthAttachment;
	renderInfo.pStencilAttachment = nullptr;

	return renderInfo;
}
