#include "ShapeSystem.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/MaterialComponent.h"

void ShapeSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto [shapePool, transformPool, materialPool] = registry->GetPools<ShapeComponent, TransformComponent, MaterialComponent>();

	if (!shapePool)
		return;

	std::for_each(std::execution::par, shapePool->GetDenseEntities().begin(), shapePool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			if (shapePool->GetBitset(entity).test(UPDATE_BIT) ||
				(transformPool && transformPool->HasComponent(entity) && transformPool->GetBitset(entity).test(INDEX_CHANGED_BIT)) ||
				(materialPool && materialPool->HasComponent(entity) && materialPool->GetBitset(entity).test(INDEX_CHANGED_BIT))
			){
				shapePool->GetBitset(entity).set(CHANGED_BIT, true);
				shapePool->GetComponent(entity)->versionID++;
			}
		}
	);
}

void ShapeSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto shapePool = registry->GetPool<ShapeComponent>();

	if (!shapePool)
		return;

	std::for_each(std::execution::par, shapePool->GetDenseEntities().begin(), shapePool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			shapePool->GetBitset(entity).reset();
		}
	);
}

void ShapeSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto [shapePool, transformPool, materialPool] = registry->GetPools<ShapeComponent, TransformComponent, MaterialComponent>();

	if (!shapePool)
		return;

	auto componentBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("ShapeRenderIndicesData", frameIndex);
	auto bufferHandler = static_cast<RenderIndicesGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, shapePool->GetDenseEntities().begin(), shapePool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			auto shapeIndex = shapePool->GetIndex(entity);
			auto shapeComponent = shapePool->GetComponent(entity);

			if (componentBuffer->versions[shapeIndex] != shapeComponent->versionID)
			{
				componentBuffer->versions[shapeIndex] = shapeComponent->versionID;

				bufferHandler[shapeIndex] = RenderIndicesGPU{
					.entityIndex = entity,
					.transformIndex = transformPool && transformPool->HasComponent(entity) ? transformPool->GetIndex(entity) : NULL_ENTITY,
					.materialIndex = materialPool && materialPool->HasComponent(entity) ? materialPool->GetIndex(entity) : NULL_ENTITY,
					.receiveShadow = shapeComponent->receiveShadow ? 1u : 0u
				};
			}
		}
	);
}
