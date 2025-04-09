#include "CameraSystem.h"
#include "Engine/Managers/InputManager.h"
#include "Engine/Components/CameraComponent.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>

void CameraSystem::OnStart(std::shared_ptr<Registry> registry)
{
}

void CameraSystem::OnUpdate(std::shared_ptr<Registry> registry, float deltaTime)
{
	auto inputManager = InputManager::Instance();
	auto [transformPool, cameraPool] = registry->GetPools<TransformComponent, CameraComponent>();

	if (!transformPool || !cameraPool)
		return;

	std::for_each(std::execution::seq, cameraPool->GetDenseEntities().begin(), cameraPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void
		{
			if (inputManager->IsKeyHeld(KEY_W) || inputManager->IsKeyHeld(KEY_S) || inputManager->IsKeyHeld(KEY_A) || inputManager->IsKeyHeld(KEY_D) || inputManager->IsButtonHeld(BUTTON_RIGHT))
				cameraPool->GetBitset(entity).set(UPDATE_BIT, true);

			if (cameraPool->GetBitset(entity).test(UPDATE_BIT))
			{
				auto transformComponent = transformPool->GetComponent(entity);
				auto cameraComponent = cameraPool->GetComponent(entity);

				float forward = 0;
				float sideways = 0;
				float distance = 0;

				if (inputManager->IsKeyHeld(KEY_W))
					forward = 1;
				if (inputManager->IsKeyHeld(KEY_S))
					forward = -1;
				if (inputManager->IsKeyHeld(KEY_D))
					sideways = 1;
				if (inputManager->IsKeyHeld(KEY_A))
					sideways = -1;
				if (inputManager->IsButtonHeld(BUTTON_RIGHT))
				{
					auto deltaPos = inputManager->GetMouseDelta();
					cameraComponent->yaw += cameraComponent->sensitivity * deltaPos.first;
					cameraComponent->pitch += cameraComponent->sensitivity * -1 * deltaPos.second;
					cameraComponent->pitch = glm::clamp<float>(cameraComponent->pitch, -89.f, 89.f);
				}

				glm::vec3 direction{
					glm::cos(glm::radians(cameraComponent->yaw)) * glm::cos(glm::radians(cameraComponent->pitch)),
					glm::sin(glm::radians(cameraComponent->pitch)),
					glm::sin(glm::radians(cameraComponent->yaw)) * glm::cos(glm::radians(cameraComponent->pitch))
				};

				cameraComponent->direction = glm::normalize(direction);
				cameraComponent->right = glm::normalize(glm::cross(cameraComponent->direction, cameraComponent->up));
				cameraComponent->position += (forward * cameraComponent->direction + sideways * cameraComponent->right) * cameraComponent->speed * deltaTime;
				cameraComponent->target = cameraComponent->position + cameraComponent->direction;

				cameraComponent->view = glm::lookAt(cameraComponent->position, cameraComponent->target, cameraComponent->up);
				cameraComponent->viewInv = glm::inverse(cameraComponent->view);

				cameraComponent->proj = glm::perspective(glm::radians(cameraComponent->fov), cameraComponent->width / cameraComponent->height, cameraComponent->nearPlane, cameraComponent->farPlane);
				cameraComponent->proj[1][1] *= -1;
				cameraComponent->projInv = glm::inverse(cameraComponent->proj);

				cameraComponent->viewProj = cameraComponent->proj * cameraComponent->view;
				cameraComponent->viewProjInv = glm::inverse(cameraComponent->viewProj);

				cameraPool->GetBitset(entity).set(UPDATE_BIT, false);
				cameraPool->GetBitset(entity).set(CHANGED_BIT, true);
				cameraComponent->versionID++;
			}
		}
	);
}

void CameraSystem::OnFinish(std::shared_ptr<Registry> registry)
{
	auto cameraPool = registry->GetPool<CameraComponent>();

	if (!cameraPool)
		return;

	std::for_each(std::execution::par, cameraPool->GetDenseEntities().begin(), cameraPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			cameraPool->GetBitset(entity).set(REGENERATE_BIT, false);
			cameraPool->GetBitset(entity).set(UPDATE_BIT, false);
			cameraPool->GetBitset(entity).set(CHANGED_BIT, false);
		}
	);
}

void CameraSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ComponentBufferManager> componentBufferManager, uint32_t frameIndex)
{
	auto cameraPool = registry->GetPool<CameraComponent>();

	if (!cameraPool)
		return;

	auto componentBuffer = componentBufferManager->GetComponentBuffer("CameraComponentGPU", frameIndex);
	auto bufferHandler = static_cast<CameraComponentGPU*>(componentBuffer->buffer->GetHandler());

	std::for_each(std::execution::seq, cameraPool->GetDenseEntities().begin(), cameraPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			auto cameraComponent = cameraPool->GetComponent(entity);
			auto index = cameraPool->GetIndex(entity);

			if (componentBuffer->versions[index] != cameraComponent->versionID)
			{
				componentBuffer->versions[index] = cameraComponent->versionID;
				bufferHandler[index] = CameraComponentGPU(*cameraComponent);
			}
		}
	);
}
