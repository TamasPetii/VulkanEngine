#include "vk_images.h"

void Vkimages::TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier2 imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.oldLayout = currentLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.pNext = nullptr;

	imageMemoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageMemoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageMemoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange = Vkinit::ImageSubResourceRange(aspectMask);

	VkDependencyInfo depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageMemoryBarrier;
	depInfo.pNext = nullptr;

	vkCmdPipelineBarrier2(commandBuffer, &depInfo);
}

void Vkimages::CopyImageToImage(VkCommandBuffer commandBuffer, VkImage imageSource, VkImage imageDestination, VkExtent2D sourceSize, VkExtent2D destinationSize)
{
	VkImageBlit2 imageBlit{};
	imageBlit.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	imageBlit.pNext = nullptr;

	imageBlit.srcOffsets[1].x = sourceSize.width;
	imageBlit.srcOffsets[1].y = sourceSize.height;
	imageBlit.srcOffsets[1].z = 1;
	imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlit.srcSubresource.baseArrayLayer = 0;
	imageBlit.srcSubresource.layerCount = 1;
	imageBlit.srcSubresource.mipLevel = 0;

	imageBlit.dstOffsets[1].x = destinationSize.width;
	imageBlit.dstOffsets[1].y = destinationSize.height;
	imageBlit.dstOffsets[1].z = 1;
	imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlit.dstSubresource.baseArrayLayer = 0;
	imageBlit.dstSubresource.layerCount = 1;
	imageBlit.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitImageInfo{};
	blitImageInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitImageInfo.srcImage = imageSource;
	blitImageInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitImageInfo.dstImage = imageDestination;
	blitImageInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitImageInfo.filter = VK_FILTER_LINEAR;
	blitImageInfo.regionCount = 1;
	blitImageInfo.pRegions = &imageBlit;

	vkCmdBlitImage2(commandBuffer, &blitImageInfo);
}
