#include "TransformSystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include "Engine/Vulkan/Buffer.h"
#include <random>

void TransformSystem::OnStart(std::shared_ptr<Registry> registry)
{
}

void TransformSystem::OnUpdate(std::shared_ptr<Registry> registry)
{
	auto [transformPool] = registry->GetPools<TransformComponent>();
	if (!transformPool)
		return;

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	std::for_each(std::execution::par, transformPool->GetDenseEntities().begin(), transformPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			if (transformPool->GetBitset(entity).test(UPDATE_BIT))
			{
				auto index = transformPool->GetIndex(entity);
				auto transformComponent = transformPool->GetComponent(entity);

				transformComponent->scale = glm::vec3(dist(rng), dist(rng), dist(rng));
				transformComponent->rotation = glm::vec3(dist(rng), dist(rng), dist(rng));
				transformComponent->translation = glm::vec3(dist(rng), dist(rng), dist(rng));

				glm::mat4& transform = transformComponent->transform;
				transform = glm::mat4(1.0f);
				transform = glm::translate(transform, transformComponent->translation);
				transform = glm::rotate(transform, glm::radians(transformComponent->rotation.x), glm::vec3(1, 0, 0));
				transform = glm::rotate(transform, glm::radians(transformComponent->rotation.y), glm::vec3(0, 1, 0));
				transform = glm::rotate(transform, glm::radians(transformComponent->rotation.z), glm::vec3(0, 0, 1));
				transform = glm::scale(transform, transformComponent->scale);

				transformComponent->transformIT = glm::inverse(glm::transpose(transform));
				transformComponent->versionID++;

				transformPool->GetBitset(entity).set(REGENERATE_BIT, false);
				transformPool->GetBitset(entity).set(UPDATE_BIT, false);
				transformPool->GetBitset(entity).set(CHANGED_BIT, true);
			}
		}
	);
}

void TransformSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto [transformPool] = registry->GetPools<TransformComponent>();

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
	auto [transformPool] = registry->GetPools<TransformComponent>();

	if (!transformPool)
		return;

	auto componentBuffer = componentBufferManager->GetComponentBuffer("TransformComponentGPU", frameIndex);
	auto bufferHandler = static_cast<TransformComponentGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::par, transformPool->GetDenseEntities().begin(), transformPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			auto index = transformPool->GetIndex(entity);
			auto transformComponent = transformPool->GetComponent(entity);

			if (componentBuffer->versions[index] != transformComponent->versionID)
			{
				auto index = transformPool->GetIndex(entity);
				auto transformComponent = transformPool->GetComponent(entity);

				componentBuffer->versions[index] = transformComponent->versionID;
				bufferHandler[index] = TransformComponentGPU(*transformComponent);
			}
		}
	);
}
