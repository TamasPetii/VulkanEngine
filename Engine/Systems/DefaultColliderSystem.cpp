#include "DefaultColliderSystem.h"

#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/ModelComponent.h"

void DefaultColliderSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto [defaultColliderPool, transformPool, shapePool, modelPool] = registry->GetPools<DefaultColliderComponent, TransformComponent, ShapeComponent, ModelComponent>();

	if (!defaultColliderPool || !transformPool)
		return;

	std::for_each(std::execution::par, defaultColliderPool->GetDenseIndices().begin(), defaultColliderPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void
		{
			bool isShape = shapePool && shapePool->HasComponent(entity) && shapePool->GetData(entity)->shape != nullptr;
			bool isModel = modelPool && modelPool->HasComponent(entity) && modelPool->GetData(entity)->model != nullptr;
			bool shapeChanged = isShape && shapePool->IsBitSet<CHANGED_BIT>(entity);
			bool modelChanged = isModel && modelPool->IsBitSet<CHANGED_BIT>(entity);

			if (transformPool->HasComponent(entity) && (isShape || modelPool) && (defaultColliderPool->IsBitSet<UPDATE_BIT>(entity) || transformPool->IsBitSet<CHANGED_BIT>(entity) || shapeChanged || modelChanged))
			{
				auto defaultColliderComponent = defaultColliderPool->GetData(entity);
				auto transformComponent = transformPool->GetData(entity);

				std::shared_ptr<BoundingVolume> boundingVolume = isShape ? std::static_pointer_cast<BoundingVolume>(shapePool->GetData(entity)->shape) : std::static_pointer_cast<BoundingVolume>(modelPool->GetData(entity)->model);

				glm::vec3 maxPosition{ std::numeric_limits<float>::lowest() };
				glm::vec3 minPosition{ std::numeric_limits<float>::max() };

				for (unsigned int i = 0; i < 8; ++i)
				{
					defaultColliderComponent->obbPositions[i] = glm::vec3(transformComponent->transform * glm::vec4(boundingVolume->obbPositions[i], 1));

					maxPosition = glm::max(maxPosition, defaultColliderComponent->obbPositions[i]);
					minPosition = glm::min(minPosition, defaultColliderComponent->obbPositions[i]);
				}

				defaultColliderComponent->aabbMin = minPosition;
				defaultColliderComponent->aabbMax = maxPosition;
				defaultColliderComponent->aabbOrigin = 0.5f * (minPosition + maxPosition);
				defaultColliderComponent->aabbExtents = 0.5f * (maxPosition - minPosition);

				defaultColliderComponent->origin = transformComponent->transform * glm::vec4(boundingVolume->aabbOrigin, 1);
				defaultColliderComponent->radius = glm::max(glm::max(transformComponent->scale.x, transformComponent->scale.y), transformComponent->scale.z) * glm::length(boundingVolume->aabbExtents);

				defaultColliderPool->SetBit<CHANGED_BIT>(entity);
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

	auto componentBufferSphere = resourceManager->GetComponentBufferManager()->GetComponentBuffer("DefaultColliderSphereData", frameIndex);
	auto bufferHandlerSphere = static_cast<glm::mat4*>(componentBufferSphere->buffer->GetHandler());

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
					
					bufferHandlerAabb[defaultColliderIndex] = glm::translate(defaultColliderComponent->aabbOrigin) * glm::scale(defaultColliderComponent->aabbExtents);
					bufferHandlerObb[defaultColliderIndex] = transformComponent->transform * glm::translate(boundingVolume->aabbOrigin) * glm::scale(boundingVolume->aabbExtents);
					bufferHandlerSphere[defaultColliderIndex] = glm::translate(defaultColliderComponent->origin) * glm::scale(glm::vec3(defaultColliderComponent->radius));
				}
			}
		}
	);
}