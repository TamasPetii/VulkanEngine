#include "ModelSystem.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/TransformComponent.h"

void ModelSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto [modelPool, transformPool] = registry->GetPools<ModelComponent, TransformComponent>();

	if (!modelPool)
		return;

	std::for_each(std::execution::par, modelPool->GetDenseEntities().begin(), modelPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			if (modelPool->GetBitset(entity).test(UPDATE_BIT) ||
				(transformPool && transformPool->HasComponent(entity) && transformPool->GetBitset(entity).test(INDEX_CHANGED_BIT))
				) {
				modelPool->GetBitset(entity).set(CHANGED_BIT, true);
				modelPool->GetComponent(entity)->versionID++;
			}
		}
	);
}

void ModelSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto modelPool = registry->GetPool<ModelComponent>();

	if (!modelPool)
		return;

	std::for_each(std::execution::par, modelPool->GetDenseEntities().begin(), modelPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			modelPool->GetComponent(entity)->toRender = false;
			modelPool->GetBitset(entity).reset();
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

	std::for_each(std::execution::par, modelPool->GetDenseEntities().begin(), modelPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			auto modelIndex = modelPool->GetIndex(entity);
			auto modelComponent = modelPool->GetComponent(entity);

			if (componentBuffer->versions[modelIndex] != modelComponent->versionID)
			{
				componentBuffer->versions[modelIndex] = modelComponent->versionID;

				bufferHandler[modelIndex] = RenderIndicesGPU{
					.entityIndex = entity,
					.transformIndex = transformPool && transformPool->HasComponent(entity) ? transformPool->GetIndex(entity) : NULL_ENTITY,
					.materialIndex = NULL_ENTITY,
					.receiveShadow = modelComponent->receiveShadow ? 1u : 0u
				};
			}
		}
	);
}
