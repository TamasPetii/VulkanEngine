#include "GeometryRenderer.h"

void GeometryRenderer::Render(VkCommandBuffer commandBuffer, std::shared_ptr<Vk::Buffer> vertexBuffer, std::shared_ptr<Vk::Buffer> indexBuffer)
{
	auto renderContext = RenderContext::GetContext();
	auto vulkanContext = Vk::VulkanContext::GetContext();
	uint32_t framesInFlightIndex = renderContext->GetFramesInFlightIndex();

	auto device = vulkanContext->GetDevice();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto frameBuffer = renderContext->GetFrameBuffer("main", framesInFlightIndex);

	VkClearValue clearValue;
	clearValue.color.float32[0] = 0.f;
	clearValue.color.float32[1] = 0.f;
	clearValue.color.float32[2] = 0.f;
	clearValue.color.float32[3] = 0.f;
	clearValue.depthStencil.depth = 1.0f;
	clearValue.depthStencil.stencil = 0.0f;

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("color")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("normal")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("depth")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

	VkRenderingAttachmentInfo colorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("color")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &clearValue);
	VkRenderingAttachmentInfo normalAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("normal")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &clearValue);
	VkRenderingAttachmentInfo depthAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("depth")->GetImageView(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, &clearValue);

	std::vector<VkRenderingAttachmentInfo> renderTargetAttachments = { colorAttachment , normalAttachment };
	VkRenderingInfo renderingInfo = Vk::DynamicRendering::BuildRenderingInfo(frameBuffer->GetSize(), renderTargetAttachments, &depthAttachment);

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderContext->GetGraphicsPipeline("DeferredPre")->GetPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)frameBuffer->GetSize().width;
	viewport.height = (float)frameBuffer->GetSize().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = frameBuffer->GetSize();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdPushConstants(commandBuffer, renderContext->GetGraphicsPipeline("DeferredPre")->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VkDeviceAddress), &vertexBuffer->GetAddress());
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer->Value(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);

	vkCmdEndRendering(commandBuffer);
}
