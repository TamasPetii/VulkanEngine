#include "FrustumCullingSystem.h"
#include "Engine/Components/DefaultColliderComponent.h"
#include "Engine/Components/CameraComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Physics/Collider/FrustumCollider.h"
#include "Engine/Physics/Simplex.h"
#include "Engine/Physics/Tester/TesterAABB.h"
#include "Engine/Physics/Tester/TesterGJK.h"
#include "Engine/Physics/Tester/TesterFrustum.h"
#include "Engine/Systems/CameraSystem.h"

void FrustumCullingSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto cameraPool = registry->GetPool<CameraComponent>();
	if (!cameraPool)
		return;

	Entity cameraEntity = CameraSystem::GetMainCameraEntity(registry);
	auto cameraComponent = cameraPool->GetComponent(cameraEntity);

	FrustumCollider frustumCollider{};

	float fovY = glm::radians(cameraComponent->fov);
	float aspectRatio = cameraComponent->width / cameraComponent->height;
	float halfV = cameraComponent->farPlane * tanf(fovY * 0.5f);
	float halfH = halfV * aspectRatio;

	//NearFace
	frustumCollider.faces[0] = FrustumFace(cameraComponent->direction, cameraComponent->position + cameraComponent->direction * cameraComponent->nearPlane);
	
	//RightFace
	frustumCollider.faces[1] = FrustumFace(-glm::cross(glm::normalize(cameraComponent->direction * cameraComponent->farPlane + cameraComponent->right * halfH), cameraComponent->up), cameraComponent->position);

	//LeftFace
	frustumCollider.faces[2] = FrustumFace(-glm::cross(cameraComponent->up, glm::normalize(cameraComponent->direction * cameraComponent->farPlane - cameraComponent->right * halfH)), cameraComponent->position);

	//TopFace
	frustumCollider.faces[3] = FrustumFace(-glm::cross(cameraComponent->right, glm::normalize(cameraComponent->direction * cameraComponent->farPlane + cameraComponent->up * halfV)), cameraComponent->position);

	//BottomFace
	frustumCollider.faces[4] = FrustumFace(-glm::cross(glm::normalize(cameraComponent->direction * cameraComponent->farPlane - cameraComponent->up * halfV), cameraComponent->right), cameraComponent->position);

	//FarFace
	frustumCollider.faces[5] = FrustumFace(-cameraComponent->direction, cameraComponent->position + cameraComponent->direction * cameraComponent->farPlane);

	DefaultColliderCulling(registry, &frustumCollider);
}

void FrustumCullingSystem::OnFinish(std::shared_ptr<Registry> registry)
{
}

void FrustumCullingSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
}

void FrustumCullingSystem::DefaultColliderCulling(std::shared_ptr<Registry> registry, FrustumCollider* cameraCollider)
{
	auto [defaultColliderPool, transformPool, shapePool, modelPool] = registry->GetPools<DefaultColliderComponent, TransformComponent, ShapeComponent, ModelComponent>();
	if (!defaultColliderPool || !transformPool)
		return;

	std::for_each(std::execution::par, defaultColliderPool->GetDenseEntities().begin(), defaultColliderPool->GetDenseEntities().end(),
		[&](const Entity& entity) -> void {
			bool hasShape = shapePool && shapePool->HasComponent(entity) && shapePool->GetComponent(entity)->shape != nullptr;
			bool hasModel = modelPool && modelPool->HasComponent(entity) && modelPool->GetComponent(entity)->model != nullptr;

			if (transformPool->HasComponent(entity) && (hasShape || hasModel))
			{
				auto defaultColliderComponent = defaultColliderPool->GetComponent(entity);

				Simplex simplex;
				if (TesterFrustum::Test(cameraCollider, defaultColliderComponent))
				{
					RenderComponent* renderComponent = hasShape ? static_cast<RenderComponent*>(shapePool->GetComponent(entity)) : static_cast<RenderComponent*>(modelPool->GetComponent(entity));
					renderComponent->toRender = true;
				}
			}
		}
	);
}
