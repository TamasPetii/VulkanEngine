#include "FrustumCullingSystem.h"
#include "Engine/Components/DefaultColliderComponent.h"
#include "Engine/Components/CameraComponent.h"
#include "Engine/Components/ShapeComponent.h"
#include "Engine/Components/ModelComponent.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Physics/Simplex.h"
#include "Engine/Physics/Tester/TesterAABB.h"
#include "Engine/Physics/Tester/TesterGJK.h"
#include "Engine/Systems/CameraSystem.h"

void FrustumCullingSystem::OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime)
{
	auto cameraPool = registry->GetPool<CameraComponent>();
	if (!cameraPool)
		return;

	Entity cameraEntity = CameraSystem::GetMainCameraEntity(registry);
	auto cameraComponent = cameraPool->GetComponent(cameraEntity);

	DefaultColliderComponent frustumCollider;
	glm::vec3 cameraAabbMin = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 cameraAabbMax = glm::vec3(std::numeric_limits<float>::lowest());

	int index = 0;
	for (int x = 0; x < 2; ++x)
	{
		for (int y = 0; y < 2; ++y)
		{
			for (int z = 0; z < 2; ++z)
			{
				float x_ncd = 2.0f * x - 1.0f;
				float y_ncd = 2.0f * y - 1.0f;
				float z_ncd = 2.0f * z - 1.0f;
				glm::vec4 pt = cameraComponent->viewProjInv * glm::vec4(x_ncd, y_ncd, z_ncd, 1.0f);
				pt /= pt.w;
				frustumCollider.obbPositions[index++] = pt;
				cameraAabbMin = glm::min(cameraAabbMin, glm::vec3(pt));
				cameraAabbMax = glm::max(cameraAabbMax, glm::vec3(pt));
			}
		}
	}

	frustumCollider.aabbMin = cameraAabbMin;
	frustumCollider.aabbMax = cameraAabbMax;

	DefaultColliderCulling(registry, &frustumCollider);
}

void FrustumCullingSystem::OnFinish(std::shared_ptr<Registry> registry)
{
}

void FrustumCullingSystem::OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex)
{
}

void FrustumCullingSystem::DefaultColliderCulling(std::shared_ptr<Registry> registry, DefaultColliderComponent* cameraCollider)
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
				if (TesterAABB::Test(*defaultColliderComponent, *cameraCollider) && TesterGJK::Test(defaultColliderComponent, static_cast<ColliderOBB*>(cameraCollider), simplex))
				{
					RenderComponent* renderComponent = hasShape ? static_cast<RenderComponent*>(shapePool->GetComponent(entity)) : static_cast<RenderComponent*>(modelPool->GetComponent(entity));
					renderComponent->toRender = true;
				}
			}
		}
	);
}
