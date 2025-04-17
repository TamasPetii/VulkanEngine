#include "ModelSystem.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/TransformComponent.h"

void ModelSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto [modelPool, transformPool] = registry->GetPools<ModelComponent, TransformComponent>();

	if (!modelPool)
		return;

	std::for_each(std::execution::par, modelPool->GetDenseIndices().begin(), modelPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			if (modelPool->IsBitSet<UPDATE_BIT>(entity) ||
				(transformPool && transformPool->HasComponent(entity) && transformPool->IsBitSet<INDEX_CHANGED_BIT>(entity)))
			{
				modelPool->SetBit<CHANGED_BIT>(entity);
				modelPool->GetData(entity)->versionID++;
			}
		}
	);
}

void ModelSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto modelPool = registry->GetPool<ModelComponent>();

	if (!modelPool)
		return;

	std::for_each(std::execution::par, modelPool->GetDenseIndices().begin(), modelPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			modelPool->GetData(entity)->toRender = false;
			modelPool->GetBitset(entity)->reset();
		}
	);
}

void ModelSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto [modelPool, transformPool] = registry->GetPools<ModelComponent, TransformComponent>();

	if (!modelPool)
		return;

	auto componentBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("ModelRenderIndicesData", frameIndex);
	auto bufferHandler = static_cast<RenderIndicesGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, modelPool->GetDenseIndices().begin(), modelPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto modelIndex = modelPool->GetDenseIndex(entity);
			auto modelComponent = modelPool->GetData(entity);

			if (componentBuffer->versions[modelIndex] != modelComponent->versionID)
			{
				componentBuffer->versions[modelIndex] = modelComponent->versionID;

				bufferHandler[modelIndex] = RenderIndicesGPU{
					.entityIndex = entity,
					.transformIndex = transformPool && transformPool->HasComponent(entity) ? transformPool->GetDenseIndex(entity) : NULL_ENTITY,
					.materialIndex = NULL_ENTITY,
					.receiveShadow = modelComponent->receiveShadow ? 1u : 0u
				};
			}
		}
	);
}
