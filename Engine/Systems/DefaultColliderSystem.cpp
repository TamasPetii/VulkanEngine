#include "DefaultColliderSystem.h"

#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/DefaultColliderComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/ModelComponent.h"

#include <future>
#include <thread>

void DefaultColliderSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto futureShape = std::async(std::launch::async, &DefaultColliderSystem::UpdateDefaultComponentsWithShapes, this, registry, resourceManager);
	auto futureModel = std::async(std::launch::async, &DefaultColliderSystem::UpdateDefaultComponentsWithModels, this, registry, resourceManager);

	futureShape.get();
	futureModel.get();
}

void DefaultColliderSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto [defaultColliderPool, transformPool] = registry->GetPools<DefaultColliderComponent, TransformComponent>();

	if (!defaultColliderPool || !transformPool)
		return;

	std::for_each(std::execution::par, defaultColliderPool->GetDenseIndices().begin(), defaultColliderPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			defaultColliderPool->GetBitset(entity)->reset();
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

	std::for_each(std::execution::par, defaultColliderPool->GetDenseIndices().begin(), defaultColliderPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			if (transformPool->HasComponent(entity))
			{
				auto defaultColliderIndex = defaultColliderPool->GetDenseIndex(entity);
				auto defaultColliderComponent = defaultColliderPool->GetData(entity);
				auto transformComponent = transformPool->GetData(entity);

				if (componentBufferAabb->versions[defaultColliderIndex] != defaultColliderComponent->versionID ||
					componentBufferObb->versions[defaultColliderIndex] != defaultColliderComponent->versionID)
				{
					bool isShape = shapePool && shapePool->HasComponent(entity) && shapePool->GetData(entity)->shape != nullptr;
					std::shared_ptr<BoundingVolume> boundingVolume = isShape ? std::static_pointer_cast<BoundingVolume>(shapePool->GetData(entity)->shape) : std::static_pointer_cast<BoundingVolume>(modelPool->GetData(entity)->model);

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
			if (defaultColliderPool->IsBitSet<UPDATE_BIT>(entity) || transformPool->IsBitSet<CHANGED_BIT>(entity) || shapePool->IsBitSet<CHANGED_BIT>(entity))
			{
				if (auto shape = shapePool->GetData(entity)->shape)
				{
					auto defaultColliderComponent = defaultColliderPool->GetData(entity);
					auto transformComponent = transformPool->GetData(entity);

					defaultColliderComponent->origin = transformComponent->transform * glm::vec4(shape->aabbOrigin, 1);
					defaultColliderComponent->radius = glm::max(glm::max(transformComponent->scale.x, transformComponent->scale.y), transformComponent->scale.z) * glm::length(shape->aabbExtents);

					defaultColliderPool->SetBit<CHANGED_BIT>(entity);
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
			if (defaultColliderPool->IsBitSet<UPDATE_BIT>(entity) || transformPool->IsBitSet<CHANGED_BIT>(entity) || modelPool->IsBitSet<CHANGED_BIT>(entity))
			{
				if (auto model = modelPool->GetData(entity)->model)
				{
					auto defaultColliderComponent = defaultColliderPool->GetData(entity);
					auto transformComponent = transformPool->GetData(entity);

					defaultColliderComponent->origin = transformComponent->transform * glm::vec4(model->aabbOrigin, 1);
					defaultColliderComponent->radius = glm::max(glm::max(transformComponent->scale.x, transformComponent->scale.y), transformComponent->scale.z) * glm::length(model->aabbExtents);

					defaultColliderPool->SetBit<CHANGED_BIT>(entity);
					defaultColliderComponent->versionID++;
				}
			}
		}
	);
}
