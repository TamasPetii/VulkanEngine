#include "PointLightSystem.h"
#include "Engine/Components/PointLightComponent.h"
#include "Engine/Components/TransformComponent.h"

void PointLightSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, float deltaTime)
{
	auto [pointLightPool, transformPool] = registry->GetPools<PointLightComponent, TransformComponent>();
	if (!pointLightPool || !transformPool)
		return;

	std::for_each(std::execution::seq, pointLightPool->GetDenseIndices().begin(), pointLightPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto& pointLightComponent = pointLightPool->GetData(entity);

			[[unlikely]]
			if (pointLightPool->IsBitSet<REGENERATE_BIT>(entity))
			{
				pointLightComponent.shadow.version++;
				pointLightPool->ResetBit<REGENERATE_BIT>(entity);
			}

			[[unlikely]]
			if (pointLightComponent.shadow.frameBuffers[frameIndex].version != pointLightComponent.shadow.version)
			{
				pointLightComponent.shadow.frameBuffers[frameIndex].version = pointLightComponent.shadow.version;

				/*
				Vk::ImageSpecification depthImageSpec;
				depthImageSpec.type = VK_IMAGE_TYPE_2D;
				depthImageSpec.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				depthImageSpec.format = VK_FORMAT_D32_SFLOAT;
				depthImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
				depthImageSpec.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				depthImageSpec.aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
				depthImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				//VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT -> Need to refactor

				Vk::FrameBufferBuilder frameBufferBuilder;
				frameBufferBuilder
					.SetSize(pointLightComponent.shadow.textureSize, pointLightComponent.shadow.textureSize)
					.AddDepthSpecification(0, depthImageSpec);

				pointLightComponent.shadow.frameBuffers[frameIndex].frameBuffer = frameBufferBuilder.BuildDynamic();
				*/

				//TODO: DYNAMIC DESCRIPTOR ARRAY UPDATE
			}
		}
	);

	std::for_each(std::execution::par_unseq, pointLightPool->GetDenseIndices().begin(), pointLightPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			if (transformPool->HasComponent(entity) && (pointLightPool->IsBitSet<UPDATE_BIT>(entity) || transformPool->IsBitSet<CHANGED_BIT>(entity)))
			{
				auto& transformComponent = transformPool->GetData(entity);
				auto& pointLightComponent = pointLightPool->GetData(entity);

				pointLightComponent.position = glm::vec3(transformComponent.transform * glm::vec4(0.f, 0.f, 0.f, 1.f));

				glm::vec3 scale;
				scale.x = glm::length(glm::vec3(transformComponent.transform[0]));
				scale.y = glm::length(glm::vec3(transformComponent.transform[1]));
				scale.z = glm::length(glm::vec3(transformComponent.transform[2]));

				pointLightComponent.radius = defaultPointLightRadius * std::max({ scale.x, scale.y, scale.z });
				
				pointLightComponent.transform = glm::mat4(1.0f);
				pointLightComponent.transform = glm::translate(pointLightComponent.transform, pointLightComponent.position);
				pointLightComponent.transform = glm::scale(pointLightComponent.transform, glm::vec3(pointLightComponent.radius));

				//Todo: ViewProj calculation

				pointLightPool->SetBit<CHANGED_BIT>(entity);
				pointLightComponent.version++;
			}
		}
	);
}

void PointLightSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto pointLightPool = registry->GetPool<PointLightComponent>();

	if (!pointLightPool)
		return;

	std::for_each(std::execution::par_unseq, pointLightPool->GetDenseIndices().begin(), pointLightPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			pointLightPool->GetData(entity).toRender = false;

			[[unlikely]]
			if (pointLightPool->IsBitSet<CHANGED_BIT>(entity))
				pointLightPool->GetBitset(entity).reset();
		}
	);
}

void PointLightSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto pointLightPool = registry->GetPool<PointLightComponent>();

	if (!pointLightPool)
		return;

	auto pointLightComponentBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("PointLightData", frameIndex);
	auto pointLightComponentBufferHandler = static_cast<PointLightGPU*>(pointLightComponentBuffer->buffer->GetHandler());

	auto pointLightTransformBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("PointLightTransform", frameIndex);
	auto pointLightTransformBufferHandler = static_cast<glm::mat4*>(pointLightTransformBuffer->buffer->GetHandler());

	std::for_each(std::execution::par_unseq, pointLightPool->GetDenseIndices().begin(), pointLightPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto& pointLightComponent = pointLightPool->GetData(entity);
			auto pointLightIndex = pointLightPool->GetDenseIndex(entity);

			[[unlikely]]
			if (pointLightComponentBuffer->versions[pointLightIndex] != pointLightComponent.version)
			{
				pointLightComponentBuffer->versions[pointLightIndex] = pointLightComponent.version;
				pointLightComponentBufferHandler[pointLightIndex] = PointLightGPU(pointLightComponent);
			}

			[[unlikely]]
			if (pointLightTransformBuffer->versions[pointLightIndex] != pointLightComponent.version)
			{
				pointLightTransformBuffer->versions[pointLightIndex] = pointLightComponent.version;
				pointLightTransformBufferHandler[pointLightIndex] = pointLightComponent.transform;
			}
		}
	);
}
