#include "GeometryRenderer.h"

void GeometryRenderer::Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry, std::shared_ptr<ComponetBufferManager> componentBufferManager)
{
	auto renderContext = RenderContext::GetContext();
	auto vulkanContext = Vk::VulkanContext::GetContext();
	uint32_t framesInFlightIndex = renderContext->GetFramesInFlightIndex();

	auto device = vulkanContext->GetDevice();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto frameBuffer = renderContext->GetFrameBuffer("main", framesInFlightIndex);

	VkClearValue colorClearValue{};
	colorClearValue.color.float32[0] = 0.f;
	colorClearValue.color.float32[1] = 0.f;
	colorClearValue.color.float32[2] = 0.f;
	colorClearValue.color.float32[3] = 1.f;

	VkClearValue depthClearValue{};
	depthClearValue.depthStencil.depth = 1.0f;
	depthClearValue.depthStencil.stencil = 0;

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("color")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("normal")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("depth")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

	VkBufferMemoryBarrier2 bufferBarrier = {};
	bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
	bufferBarrier.srcStageMask = VK_PIPELINE_STAGE_2_HOST_BIT;
	bufferBarrier.srcAccessMask = VK_ACCESS_2_HOST_WRITE_BIT;
	bufferBarrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
	bufferBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	bufferBarrier.buffer = componentBufferManager->GetComponentBuffer("TransformComponentGPU", framesInFlightIndex)->buffer->Value();
	bufferBarrier.offset = 0;
	bufferBarrier.size = VK_WHOLE_SIZE;

	VkDependencyInfo dependencyInfo = {};
	dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependencyInfo.bufferMemoryBarrierCount = 1;
	dependencyInfo.pBufferMemoryBarriers = &bufferBarrier;
	dependencyInfo.memoryBarrierCount = 0;
	dependencyInfo.pMemoryBarriers = nullptr;
	dependencyInfo.imageMemoryBarrierCount = 0;
	dependencyInfo.pImageMemoryBarriers = nullptr;

	vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);

	VkRenderingAttachmentInfo colorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("color")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &colorClearValue);
	VkRenderingAttachmentInfo normalAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("normal")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &colorClearValue);
	VkRenderingAttachmentInfo depthAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("depth")->GetImageView(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, &depthClearValue);

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

	glm::mat4 view = glm::lookAt(glm::vec3(-3.0f, 2.0f, -3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), frameBuffer->GetSize().width / (float)frameBuffer->GetSize().height, 0.01f, 1000.0f);
	proj[1][1] *= -1;

	auto geometry = GeometryManager::GetManager()->GetShape("cube");

	static std::tuple<VkDeviceAddress, VkDeviceAddress> pushConstants;
	std::get<0>(pushConstants) = geometry->GetVertexBuffer()->GetAddress();
	std::get<1>(pushConstants) = componentBufferManager->GetComponentBuffer("TransformComponentGPU", framesInFlightIndex)->buffer->GetAddress();

	vkCmdPushConstants(commandBuffer, renderContext->GetGraphicsPipeline("DeferredPre")->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(std::tuple<VkDeviceAddress, VkDeviceAddress>), &pushConstants);
	vkCmdBindIndexBuffer(commandBuffer, geometry->GetIndexBuffer()->Value(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(commandBuffer, geometry->GetIndexCount(), 4096, 0, 0, 0);

	vkCmdEndRendering(commandBuffer);
}
