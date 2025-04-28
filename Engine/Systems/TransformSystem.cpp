#include "TransformSystem.h"
#include "Engine/Components/TransformComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void TransformSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto [transformPool, relationShipPool] = registry->GetPools<TransformComponent, Relationship>();
	if (!transformPool)
		return;

	for (uint32_t depth = 0; depth < MAX_RELATIONSHIP_DEPTH; ++depth)
	{
		auto& levelEntities = registry->GetLevels()[depth];

		if (levelEntities.empty())
			break;

		std::for_each(std::execution::par, levelEntities.begin(), levelEntities.end(),
			[&](const Entity& entity) -> void {
				auto& relationShipComponent = relationShipPool->GetData(entity);
				if (transformPool->HasComponent(entity) && (transformPool->IsBitSet<UPDATE_BIT>(entity) || (relationShipComponent.parent != NULL_ENTITY && transformPool->IsBitSet<CHANGED_BIT>(relationShipComponent.parent))))
				{
					auto& transformComponent = transformPool->GetData(entity);

					transformComponent.transform = glm::mat4(1.0f);
					transformComponent.transform = glm::translate(transformComponent.transform, transformComponent.translation);
					transformComponent.transform = glm::rotate(transformComponent.transform, glm::radians(transformComponent.rotation.z), glm::vec3(0, 0, 1));
					transformComponent.transform = glm::rotate(transformComponent.transform, glm::radians(transformComponent.rotation.y), glm::vec3(0, 1, 0));
					transformComponent.transform = glm::rotate(transformComponent.transform, glm::radians(transformComponent.rotation.x), glm::vec3(1, 0, 0));
					transformComponent.transform = glm::scale(transformComponent.transform, transformComponent.scale);
					
					if(relationShipComponent.parent != NULL_ENTITY)
						transformComponent.transform = transformPool->GetData(relationShipComponent.parent).transform * transformComponent.transform;
					
					transformComponent.transformIT = glm::inverse(glm::transpose(transformComponent.transform));

					transformPool->SetBit<CHANGED_BIT>(entity);
					transformComponent.versionID++;
				}
			}
		);
	}	
	
	/*
	//Might be better to use glm::decompose at the end in another foreach, when parent-child relation will affect transforms;
	std::for_each(std::execution::par, transformPool->GetDenseEntities().begin(), transformPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			if (transformPool->GetBitset(entity).test(CHANGED_BIT))
			{
				auto index = transformPool->GetIndex(entity);
				auto transformComponent = transformPool->GetComponent(entity);
				
				glm::vec3 scale, translation, skew;
				glm::vec4 perspective;
				glm::quat rotation;
				glm::decompose(transformComponent->transform, scale, rotation, translation, skew, perspective);

				transformComponent->fullTranslation = translation;
				transformComponent->fullRotation = glm::degrees(glm::eulerAngles(rotation));
				transformComponent->fullScale = scale;

				//Translation
				transformComponent->fullTranslationMatrix = glm::translate(glm::mat4(1.0f), transformComponent->fullTranslation);
		
				//Rotation
				transformComponent->fullRotationMatrix = glm::mat4(1.0f);
				transformComponent->fullRotationMatrix = glm::rotate(transformComponent->fullRotationMatrix, glm::radians(transformComponent->fullRotation.z), glm::vec3(0, 0, 1));
				transformComponent->fullRotationMatrix = glm::rotate(transformComponent->fullRotationMatrix, glm::radians(transformComponent->fullRotation.y), glm::vec3(0, 1, 0));
				transformComponent->fullRotationMatrix = glm::rotate(transformComponent->fullRotationMatrix, glm::radians(transformComponent->fullRotation.x), glm::vec3(1, 0, 0));
				
				//Scale
				transformComponent->fullScaleMatrix = glm::scale(glm::mat4(1.0f), transformComponent->fullScale);
			}
		}
	);
	*/
}

void TransformSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto transformPool = registry->GetPool<TransformComponent>();

	if (!transformPool)
		return;

	std::for_each(std::execution::par, transformPool->GetDenseIndices().begin(), transformPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			transformPool->GetBitset(entity).reset();
		}
	);
}

void TransformSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
	auto transformPool = registry->GetPool<TransformComponent>();

	if (!transformPool)
		return;

	auto componentBuffer = resourceManager->GetComponentBufferManager()->GetComponentBuffer("TransformData", frameIndex);
	auto bufferHandler = static_cast<TransformComponentGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, transformPool->GetDenseIndices().begin(), transformPool->GetDenseIndices().end(),
		[&](const Entity& entity) -> void {
			auto& transformComponent = transformPool->GetData(entity);
			auto transformIndex = transformPool->GetDenseIndex(entity);

			if (componentBuffer->versions[transformIndex] != transformComponent.versionID)
			{
				componentBuffer->versions[transformIndex] = transformComponent.versionID;
				bufferHandler[transformIndex] = TransformComponentGPU(transformComponent);
			}
		}
	);
}
