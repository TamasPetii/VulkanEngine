#include "TransformSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Components/TransformComponent.h"
#include <chrono>

void TransformSystem::OnStart(std::shared_ptr<Registry> registry)
{
}

void TransformSystem::OnUpdate(std::shared_ptr<Registry> registry, float deltaTime)
{
	auto transformPool = registry->GetPool<TransformComponent>();
	if (!transformPool)
		return;

	std::for_each(std::execution::par, transformPool->GetDenseEntities().begin(), transformPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			if (transformPool->GetBitset(entity).test(UPDATE_BIT))
			{
				auto index = transformPool->GetIndex(entity);
				auto transformComponent = transformPool->GetComponent(entity);

				transformComponent->transform = glm::mat4(1.0f);
				transformComponent->transform = glm::translate(transformComponent->transform, transformComponent->translation);
				transformComponent->transform = glm::rotate(transformComponent->transform, glm::radians(transformComponent->rotation.z), glm::vec3(0, 0, 1));
				transformComponent->transform = glm::rotate(transformComponent->transform, glm::radians(transformComponent->rotation.y), glm::vec3(0, 1, 0));
				transformComponent->transform = glm::rotate(transformComponent->transform, glm::radians(transformComponent->rotation.x), glm::vec3(1, 0, 0));
				transformComponent->transform = glm::scale(transformComponent->transform, transformComponent->scale);	
				transformComponent->transformIT = glm::inverse(glm::transpose(transformComponent->transform));

				transformPool->GetBitset(entity).set(CHANGED_BIT, true);
				transformComponent->versionID++;
			}
		}
	);
}

void TransformSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto transformPool = registry->GetPool<TransformComponent>();

	if (!transformPool)
		return;

	std::for_each(std::execution::par, transformPool->GetDenseEntities().begin(), transformPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			transformPool->GetBitset(entity).set(REGENERATE_BIT, false);
			transformPool->GetBitset(entity).set(UPDATE_BIT, false);
			transformPool->GetBitset(entity).set(CHANGED_BIT, false);
		}
	);
}

void TransformSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ComponentBufferManager> componentBufferManager, uint32_t frameIndex)
{
	auto transformPool = registry->GetPool<TransformComponent>();

	if (!transformPool)
		return;

	auto componentBuffer = componentBufferManager->GetComponentBuffer("TransformData", frameIndex);
	auto bufferHandler = static_cast<TransformComponentGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, transformPool->GetDenseEntities().begin(), transformPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			auto index = transformPool->GetIndex(entity);
			auto transformComponent = transformPool->GetComponent(entity);

			if (componentBuffer->versions[index] != transformComponent->versionID)
			{
				componentBuffer->versions[index] = transformComponent->versionID;
				bufferHandler[index] = TransformComponentGPU(*transformComponent);
			}
		}
	);
}
