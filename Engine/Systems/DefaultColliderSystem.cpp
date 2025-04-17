#include "DefaultColliderSystem.h"

#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/DefaultColliderComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/ModelComponent.h"

void DefaultColliderSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	UpdateDefaultComponentsWithShapes(registry, resourceManager);
	UpdateDefaultComponentsWithModels(registry, resourceManager);
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

void DefaultColliderSystem::UpdateDefaultComponentsWithShapes(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager)
{
	auto [defaultColliderPool, transformPool, shapePool] = registry->GetPools<DefaultColliderComponent, TransformComponent, ShapeComponent>();

	if (!defaultColliderPool || !transformPool || !shapePool)
		return;

	auto& entityView = registry->View<DefaultColliderComponent, TransformComponent, ShapeComponent>();
	std::for_each(std::execution::par, entityView.begin(), entityView.end(),
		[&](const Entity& entity) -> void
		{
			if (defaultColliderPool->GetBitset(entity).test(UPDATE_BIT) || transformPool->GetBitset(entity).test(CHANGED_BIT) || shapePool->GetBitset(entity).test(CHANGED_BIT))
			{
				if (auto shape = shapePool->GetComponent(entity)->shape)
				{
					auto defaultColliderComponent = defaultColliderPool->GetComponent(entity);
					auto transformComponent = transformPool->GetComponent(entity);

					defaultColliderComponent->origin = transformComponent->transform * glm::vec4(shape->aabbOrigin, 1);
					defaultColliderComponent->radius = glm::max(glm::max(transformComponent->scale.x, transformComponent->scale.y), transformComponent->scale.z) * glm::length(shape->aabbExtents);

					defaultColliderPool->GetBitset(entity).set(CHANGED_BIT, true);
					defaultColliderComponent->versionID++;
				}
			}
		}
	);
}

void DefaultColliderSystem::UpdateDefaultComponentsWithModels(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager)
{
	auto [defaultColliderPool, transformPool, modelPool] = registry->GetPools<DefaultColliderComponent, TransformComponent, ModelComponent>();

	if (!defaultColliderPool || !transformPool || !modelPool)
		return;

	auto& entityView = registry->View<DefaultColliderComponent, TransformComponent, ModelComponent>();
	std::for_each(std::execution::par, entityView.begin(), entityView.end(),
		[&](const Entity& entity) -> void
		{
			if (defaultColliderPool->GetBitset(entity).test(UPDATE_BIT) || transformPool->GetBitset(entity).test(CHANGED_BIT) || modelPool->GetBitset(entity).test(CHANGED_BIT))
			{
				if (auto model = modelPool->GetComponent(entity)->model)
				{
					auto defaultColliderComponent = defaultColliderPool->GetComponent(entity);
					auto transformComponent = transformPool->GetComponent(entity);

					defaultColliderComponent->origin = transformComponent->transform * glm::vec4(model->aabbOrigin, 1);
					defaultColliderComponent->radius = glm::max(glm::max(transformComponent->scale.x, transformComponent->scale.y), transformComponent->scale.z) * glm::length(model->aabbExtents);

					defaultColliderPool->GetBitset(entity).set(CHANGED_BIT, true);
					defaultColliderComponent->versionID++;
				}
			}
		}
	);
}
