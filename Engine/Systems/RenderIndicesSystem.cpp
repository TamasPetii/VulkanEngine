#include "RenderIndicesSystem.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/AnimationComponent.h"
#include "Engine/Components/RenderIndicesComponent.h"

void RenderIndicesSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, float deltaTime)
{
/*
	auto [renderIndicesPool, transformPool, modelPool, animationPool] = registry->GetPools<RenderIndicesComponent, TransformComponent, ModelComponent, AnimationComponent>();

	if (!renderIndicesPool)
		return;

	std::for_each(std::execution::par, renderIndicesPool->GetDenseIndices().begin(), renderIndicesPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {

			bool hasModel = modelPool && modelPool->HasComponent(entity) && modelPool->GetData(entity).model && modelPool->GetData(entity).model->state == LoadState::Ready;
			bool hasAnimation = animationPool && animationPool->HasComponent(entity) && animationPool->GetData(entity).animation && animationPool->GetData(entity).animation->state == LoadState::Ready;
			bool hasTransform = transformPool && transformPool->HasComponent(entity);

			[[unlikely]]
			if (modelPool->IsBitSet<UPDATE_BIT>(entity) || (transformPool && transformPool->HasComponent(entity) && transformPool->IsBitSet<INDEX_CHANGED_BIT>(entity)))
			{
				modelPool->SetBit<CHANGED_BIT>(entity);
				modelPool->GetData(entity).versionID++;
			}
		}
	);
	*/
}

void RenderIndicesSystem::OnFinish(std::shared_ptr<Registry> registry)
{
}

void RenderIndicesSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	/*
	auto [transformPool, modelPool, animationPool] = registry->GetPools<TransformComponent, ModelComponent, AnimationComponent>();

	if (!transformPool)
		return;

	auto renderIndicesBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("ModelRenderIndicesData", frameIndex);
	auto renderIndicesBufferHandler = static_cast<RenderIndicesGPU*>(renderIndicesBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, transformPool->GetDenseIndices().begin(), transformPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto transformIndex = transformPool->GetDenseIndex();

			[[unlikely]]
			if (renderIndicesBuffer->versions[transformIndex] != modelComponent.versionID)
			{
				bool hasAnimation = animationPool && animationPool->HasComponent(entity) && animationPool->GetData(entity).animation && animationPool->GetData(entity).animation->state == LoadState::Ready;


				renderIndicesBuffer->versions[modelIndex] = modelComponent.versionID;

				uint32_t flags = 0;
				flags |= (modelComponent.receiveShadow ? 1u : 0u) << 0;       // Bit 0
				flags |= (modelComponent.hasDirectxNormals ? 1u : 0u) << 1;    // Bit 1

				auto renderIndices = RenderIndicesGPU{
					.entityIndex = entity,
					.transformIndex = transformPool && transformPool->HasComponent(entity) ? transformPool->GetDenseIndex(entity) : UINT32_MAX,
					.materialIndex = UINT32_MAX,
					.receiveShadow = flags,
					.nodeTransformIndex = modelIndex,
					.animationIndex = hasAnimation ? animationPool->GetData(entity).animation->GetAddressArrayIndex() : UINT32_MAX,
				};

				renderIndicesBufferHandler[modelIndex] = renderIndices;
			}

			if (hasAnimation)
				nodeTransformBufferHandler[modelIndex] = animationPool->GetData(entity).nodeTransformBuffers[frameIndex].buffer->GetAddress();
			else if (modelComponent.model && modelComponent.model->state == LoadState::Ready)
				nodeTransformBufferHandler[modelIndex] = modelComponent.model->GetNodeTransformBuffer()->GetAddress();
		}
	);
	*/
}
