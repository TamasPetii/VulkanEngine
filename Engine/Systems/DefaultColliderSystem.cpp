#include "DefaultColliderSystem.h"

#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/DefaultColliderComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/ModelComponent.h"

void DefaultColliderSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto [defaultColliderPool, transformPool, shapePool, modelPool] = registry->GetPools<DefaultColliderComponent, TransformComponent, ShapeComponent, ModelComponent>();

	if (!defaultColliderPool || !transformPool)
		return;

	std::for_each(std::execution::par, defaultColliderPool->GetDenseEntities().begin(), defaultColliderPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void 
		{
			bool isShape = shapePool && shapePool->HasComponent(entity) && shapePool->GetComponent(entity)->shape != nullptr;
			bool isModel = modelPool && modelPool->HasComponent(entity) && modelPool->GetComponent(entity)->model != nullptr;
			bool shapeChanged = isShape && shapePool->GetBitset(entity).test(CHANGED_BIT);
			bool modelChanged = isModel && modelPool->GetBitset(entity).test(CHANGED_BIT);

			if (transformPool->HasComponent(entity) && (isShape || modelPool) && (defaultColliderPool->GetBitset(entity).test(UPDATE_BIT) || transformPool->GetBitset(entity).test(CHANGED_BIT) || shapeChanged || modelChanged))
			{
				auto defaultColliderComponent = defaultColliderPool->GetComponent(entity);
				auto transformComponent = transformPool->GetComponent(entity);

				std::shared_ptr<BoundingVolume> boundingVolume = isShape ? std::static_pointer_cast<BoundingVolume>(shapePool->GetComponent(entity)->shape) : std::static_pointer_cast<BoundingVolume>(modelPool->GetComponent(entity)->model);
				
				defaultColliderComponent->origin = transformComponent->transform * glm::vec4(boundingVolume->aabbOrigin, 1);
				defaultColliderComponent->radius = glm::max(glm::max(transformComponent->scale.x, transformComponent->scale.y), transformComponent->scale.z) * glm::length(boundingVolume->aabbExtents);

				defaultColliderPool->GetBitset(entity).set(CHANGED_BIT, true);
				defaultColliderComponent->versionID++;
			}
		}
	);
}

void DefaultColliderSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto [defaultColliderPool, transformPool] = registry->GetPools<DefaultColliderComponent, TransformComponent>();

	if (!defaultColliderPool || !transformPool)
		return;

	std::for_each(std::execution::par, defaultColliderPool->GetDenseEntities().begin(), defaultColliderPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			defaultColliderPool->GetBitset(entity).reset();
		}
	);
}

void DefaultColliderSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto [defaultColliderPool, transformPool, shapePool, modelPool] = registry->GetPools<DefaultColliderComponent, TransformComponent, ShapeComponent, ModelComponent>();

	if (!defaultColliderPool || !transformPool)
		return;

	auto componentBufferAabb = resourceManager->GetComponentBufferManager()->GetComponentBuffer("DefaultColliderAabbData", frameIndex);
	auto bufferHandlerAabb = static_cast<glm::mat4*>(componentBufferAabb->buffer->GetHandler());

	auto componentBufferObb = resourceManager->GetComponentBufferManager()->GetComponentBuffer("DefaultColliderObbData", frameIndex);
	auto bufferHandlerObb = static_cast<glm::mat4*>(componentBufferObb->buffer->GetHandler());

	std::for_each(std::execution::par, defaultColliderPool->GetDenseEntities().begin(), defaultColliderPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			if (transformPool->HasComponent(entity))
			{
				auto defaultColliderIndex = defaultColliderPool->GetIndex(entity);
				auto defaultColliderComponent = defaultColliderPool->GetComponent(entity);
				auto transformComponent = transformPool->GetComponent(entity);

				if (componentBufferAabb->versions[defaultColliderIndex] != defaultColliderComponent->versionID ||
					componentBufferObb->versions[defaultColliderIndex] != defaultColliderComponent->versionID)
				{
					bool isShape = shapePool && shapePool->HasComponent(entity) && shapePool->GetComponent(entity)->shape != nullptr;
					std::shared_ptr<BoundingVolume> boundingVolume = isShape ? std::static_pointer_cast<BoundingVolume>(shapePool->GetComponent(entity)->shape) : std::static_pointer_cast<BoundingVolume>(modelPool->GetComponent(entity)->model);

					componentBufferAabb->versions[defaultColliderIndex] = defaultColliderComponent->versionID;
					componentBufferObb->versions[defaultColliderIndex] = defaultColliderComponent->versionID;
					
					bufferHandlerAabb[defaultColliderIndex] = glm::translate(defaultColliderComponent->origin) * glm::scale(glm::vec3(defaultColliderComponent->radius));
					bufferHandlerObb[defaultColliderIndex] = transformComponent->transform * glm::translate(boundingVolume->aabbOrigin) * glm::scale(boundingVolume->aabbExtents);
				}
			}
		}
	);
}
