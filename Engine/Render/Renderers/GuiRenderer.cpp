#include "GuiRenderer.h"

void GuiRenderer::Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, uint32_t imageIndex, const std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)>& guiRenderFunction)
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	auto device = vulkanContext->GetDevice();
	auto swapChain = vulkanContext->GetSwapChain();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto frameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, swapChain->GetImages()[imageIndex],
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("Main")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT);

	if (guiRenderFunction)
	{
		VkRenderingAttachmentInfo guiColorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(swapChain->GetImageViews()[imageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, nullptr);
		std::vector<VkRenderingAttachmentInfo> renderTargetAttachments2 = { guiColorAttachment };
		VkRenderingInfo guiRenderingInfo = Vk::DynamicRendering::BuildRenderingInfo(swapChain->GetExtent(), renderTargetAttachments2, nullptr);

		vkCmdBeginRendering(commandBuffer, &guiRenderingInfo);

		guiRenderFunction(commandBuffer, registry, resourceManager, frameIndex);

		vkCmdEndRendering(commandBuffer);
	}
}
