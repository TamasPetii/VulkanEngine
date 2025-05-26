#include "WireframeRenderer.h"
#include "../GpuStructs.h"
#include <algorithm>
#include <execution>
#include "Engine/Components/DefaultColliderComponent.h"
#include "Engine/Components/PointLightComponent.h"

void WireframeRenderer::Render(VkCommandBuffer commandBuffer, std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	auto device = vulkanContext->GetDevice();
	auto graphicsQueue = device->GetQueue(Vk::QueueType::GRAPHICS);
	auto frameBuffer = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex);
	auto pipeline = resourceManager->GetVulkanManager()->GetGraphicsPipeline("Wireframe");
	
	VkRenderingAttachmentInfo colorAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("Main")->GetImageView(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, nullptr);
	VkRenderingAttachmentInfo depthAttachment = Vk::DynamicRendering::BuildRenderingAttachmentInfo(frameBuffer->GetImage("Depth")->GetImageView(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, nullptr);

	std::vector<VkRenderingAttachmentInfo> renderTargetAttachments = { colorAttachment };
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

	auto RenderWireframeVolume = [&](const std::string& shapeName, const std::string& bufferName, const glm::vec4& color, uint32_t count) -> void 
		{
			if (count > 0)
			{
				auto shape = resourceManager->GetGeometryManager()->GetShape(shapeName);

				BoundingVolumeRendererPushConstants pushConstants;
				pushConstants.cameraIndex = 0;
				pushConstants.cameraBufferAddress = resourceManager->GetComponentBufferManager()->GetComponentBuffer("CameraData", frameIndex)->buffer->GetAddress();
				pushConstants.transformBufferAddress = resourceManager->GetComponentBufferManager()->GetComponentBuffer(bufferName, frameIndex)->buffer->GetAddress();
				pushConstants.vertexBufferAddress = shape->GetVertexBuffer()->GetAddress();
				pushConstants.indexBufferAddress = shape->GetIndexBuffer()->GetAddress();
				pushConstants.color = color;

				vkCmdPushConstants(commandBuffer, pipeline->GetLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(BoundingVolumeRendererPushConstants), &pushConstants);
				vkCmdDraw(commandBuffer, shape->GetIndexCount(), count, 0, 0);
			}
		};

	if (auto defaultColliderPool = registry->GetPool<DefaultColliderComponent>())
	{
		uint32_t count = defaultColliderPool->GetDenseSize();

		if(GlobalConfig::WireframeConfig::showColliderAABB)
			RenderWireframeVolume("Cube", "DefaultColliderAabbData", glm::vec4(1, 1, 1, 1), count);

		if(GlobalConfig::WireframeConfig::showColliderOBB)
			RenderWireframeVolume("Cube", "DefaultColliderObbData", glm::vec4(1, 0, 0, 1), count);

		if(GlobalConfig::WireframeConfig::showColliderSphere)
			RenderWireframeVolume("ProxySphere", "DefaultColliderSphereData", glm::vec4(1, 0, 1, 1), count);
	}

	if (auto pointLightPool = registry->GetPool<PointLightComponent>())
	{
		uint32_t count = pointLightPool->GetDenseSize();

		if (GlobalConfig::WireframeConfig::showPointLights)
			RenderWireframeVolume("Cube", "PointLightTransform", glm::vec4(1, 1, 1, 1), count);
	}

	vkCmdEndRendering(commandBuffer);
}