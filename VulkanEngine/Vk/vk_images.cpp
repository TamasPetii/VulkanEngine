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
