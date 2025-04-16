#include "GeometryRenderer.h"
#include "../GpuStructs.h"
#include <algorithm>
#include <execution>

void GeometryRenderer::Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	auto device = vulkanContext->GetDevice();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto frameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex);
	auto pipeline = resourceManager->GetVulkanManager()->GetGraphicsPipeline("DeferredPre");

	VkQueryPool queryPool;
	VkQueryPoolCreateInfo queryPoolInfo{};
	queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
	queryPoolInfo.queryCount = 2;

	VK_CHECK_MESSAGE(vkCreateQueryPool(device->Value(), &queryPoolInfo, nullptr, &queryPool), "Failed to create query pool!");

	vkCmdResetQueryPool(commandBuffer, queryPool, 0, 2);
	vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool, 0);

	VkClearValue colorClearValue{};
	colorClearValue.color.float32[0] = 0.f;
	colorClearValue.color.float32[1] = 0.f;
	colorClearValue.color.float32[2] = 0.f;
	colorClearValue.color.float32[3] = 1.f;

	VkClearValue depthClearValue{};
	depthClearValue.depthStencil.depth = 1.0f;
	depthClearValue.depthStencil.stencil = 0;

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("Color")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("Normal")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);

	Vk::Image::TransitionImageLayoutDynamic(commandBuffer, frameBuffer->GetImage("Depth")->Value(),
		VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

	VkRenderingAttachmentInfo colorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("Color")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &colorClearValue);
	VkRenderingAttachmentInfo normalAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("Normal")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &colorClearValue);
	VkRenderingAttachmentInfo entityAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("Entity")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, &colorClearValue);
	VkRenderingAttachmentInfo depthAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("Depth")->GetImageView(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, &depthClearValue);

	std::vector<VkRenderingAttachmentInfo> renderTargetAttachments = { colorAttachment , normalAttachment, entityAttachment };
	VkRenderingInfo renderingInfo = Vk::DynamicRendering::BuildRenderingInfo(frameBuffer->GetSize(), renderTargetAttachments, &depthAttachment);

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

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

	auto textureDescriptorSet = resourceManager->GetVulkanManager()->GetDescriptorSet("LoadedImages");
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(), 0, 1, &textureDescriptorSet->Value(), 0, nullptr);

	RenderShapesInstanced(commandBuffer, pipeline->GetLayout(), resourceManager, frameIndex);
	RenderModelsInstanced(commandBuffer, pipeline->GetLayout(), resourceManager, frameIndex);

	vkCmdEndRendering(commandBuffer);

	vkCmdWriteTimestamp(commandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, queryPool, 1);
}

void GeometryRenderer::RenderShapesInstanced(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto geometryManager = resourceManager->GetGeometryManager();
	std::for_each(std::execution::seq, geometryManager->GetShapes().begin(), geometryManager->GetShapes().end(),
		[&](const std::pair<std::string, std::shared_ptr<Shape>>& data) -> void {
			auto shape = data.second;
			if (shape->GetInstanceCount() > 0)
			{
				GpuPushConstant pushConstants;
				pushConstants.renderMode = SHAPE_INSTANCED;
				pushConstants.cameraIndex = 0;
				pushConstants.cameraBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("CameraData", frameIndex)->buffer->GetAddress();
				pushConstants.vertexBuffer = shape->GetVertexBuffer()->GetAddress();
				pushConstants.instanceIndexBuffer = shape->GetInstanceIndexBuffer(frameIndex)->GetAddress();
				pushConstants.transformBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("TransformData", frameIndex)->buffer->GetAddress();
				pushConstants.materialBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("MaterialData", frameIndex)->buffer->GetAddress();
				pushConstants.renderIndicesBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("ShapeRenderIndicesData", frameIndex)->buffer->GetAddress();

				vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(GpuPushConstant), &pushConstants);
				vkCmdBindIndexBuffer(commandBuffer, shape->GetIndexBuffer()->Value(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(commandBuffer, shape->GetIndexCount(), shape->GetInstanceCount(), 0, 0, 0);
			}
		}
	);
}

void GeometryRenderer::RenderModelsInstanced(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto modelManager = resourceManager->GetModelManager();
	std::for_each(std::execution::seq, modelManager->GetModels().begin(), modelManager->GetModels().end(),
		[&](const std::pair<std::string, std::shared_ptr<Model>>& data) -> void {
			auto model = data.second;
			if (model->GetInstanceCount() > 0)
			{
				GpuPushConstant pushConstants;
				pushConstants.renderMode = MODEL_INSTANCED;
				pushConstants.cameraIndex = 0;
				pushConstants.cameraBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("CameraData", frameIndex)->buffer->GetAddress();
				pushConstants.vertexBuffer = model->GetVertexBuffer()->GetAddress();
				pushConstants.instanceIndexBuffer = model->GetInstanceIndexBuffer(frameIndex)->GetAddress();
				pushConstants.transformBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("TransformData", frameIndex)->buffer->GetAddress();
				pushConstants.materialBuffer = model->GetMaterialBuffer()->GetAddress();
				pushConstants.renderIndicesBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("ModelRenderIndicesData", frameIndex)->buffer->GetAddress();

				vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(GpuPushConstant), &pushConstants);
				vkCmdBindIndexBuffer(commandBuffer, model->GetIndexBuffer()->Value(), 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(commandBuffer, model->GetIndexCount(), model->GetInstanceCount(), 0, 0, 0);
			}
		}
	);
}
