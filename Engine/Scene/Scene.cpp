#include "Scene.h"
#include <random>

Scene::Scene(std::shared_ptr<ResourceManager> resourceManager) : 
	resourceManager(resourceManager)
{
	Initialize();
}

Scene::~Scene()
{
	Cleanup();
}

void Scene::Cleanup()
{
	registry.reset();
	systems.clear();
}

void Scene::Initialize()
{
	InitializeSystems();
	InitializeRegistry();
}


void Scene::Update(std::shared_ptr<Timer> frameTimer, uint32_t frameIndex)
{
	/*
	//Update Registry Transforms
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);
	auto transformPool = registry->GetPool<TransformComponent>();
	std::for_each(std::execution::par,
		transformPool->GetDenseEntities().begin(),
		transformPool->GetDenseEntities().end(),
		[&](Entity entity) -> void
		{
			auto transformComponent = transformPool->GetComponent(entity);
			transformComponent->scale = glm::vec3(glm::sin(frameTimer->GetFrameElapsedTime() * 2 * glm::pi<float>()) + 2);
			registry->GetPool<TransformComponent>()->GetBitset(entity).set(UPDATE_BIT, true);
		}
	);

	//Update Camera Size
	auto viewPortSize = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex)->GetSize();
	auto cameraComponent = registry->GetComponent<CameraComponent>(0);
	if (viewPortSize.width != cameraComponent->width || viewPortSize.height != cameraComponent->height)
	{
		cameraComponent->width = viewPortSize.width;
		cameraComponent->height = viewPortSize.height;
		registry->GetPool<CameraComponent>()->GetBitset(0).set(UPDATE_BIT, true);
	}
	*/

	auto geometry = resourceManager->GetGeometryManager()->GetShape("Cube");
	geometry->ResetInstanceCount();

	for (uint32_t i = 0; i < 500; i++)
		geometry->AddIndex({ i + 1, i + 1, i, 0 });

	UpdateSystems(frameTimer->GetFrameDeltaTime());
	FinishSystems();
}

void Scene::UpdateGPU(uint32_t frameIndex)
{
	auto geometry = resourceManager->GetGeometryManager()->GetShape("Cube");
	geometry->UploadInstanceDataToGPU(frameIndex);

	UpdateComponentBuffers(frameIndex);
	UpdateSystemsGPU(frameIndex);
}

void Scene::InitializeRegistry()
{
	registry = std::make_shared<Registry>();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	{ //Camera
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent>(entity);
		registry->AddComponents<CameraComponent>(entity);
	}

	for (uint32_t i = 0; i < 500; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, MaterialComponent>(entity);

		auto [transformComponent, materialComponent] = registry->GetComponents<TransformComponent, MaterialComponent>(entity);
		transformComponent->rotation = 180.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->translation = 100.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->scale = glm::vec3(1);

		materialComponent->color = glm::vec4(dist(rng), dist(rng), dist(rng), 1);
		materialComponent->albedo = resourceManager->GetImageManager()->LoadImage("../Assets/Texture.jpg");
	}
}

void Scene::InitializeSystems()
{
	InitSystem<TransformSystem>();
	InitSystem<CameraSystem>();
	InitSystem<MaterialSystem>();
}

void Scene::UpdateSystems(float deltaTime)
{
	UpdateSystem<TransformSystem>(deltaTime);
	UpdateSystem<CameraSystem>(deltaTime);
	UpdateSystem<MaterialSystem>(deltaTime);
}

void Scene::FinishSystems()
{
	FinishSystem<TransformSystem>();
	FinishSystem<CameraSystem>();
	FinishSystem<MaterialSystem>();
}

void Scene::UpdateSystemsGPU(uint32_t frameIndex)
{
	UpdateGpuSystem<TransformSystem>(frameIndex);
	UpdateGpuSystem<CameraSystem>(frameIndex);
	UpdateGpuSystem<MaterialSystem>(frameIndex);
}

void Scene::UpdateComponentBuffers(uint32_t frameIndex)
{
	RecalculateGpuBufferSize<TransformComponent, TransformComponentGPU>("TransformData", frameIndex);
	RecalculateGpuBufferSize<CameraComponent, CameraComponentGPU>("CameraData", frameIndex);
	RecalculateGpuBufferSize<MaterialComponent, MaterialComponentGPU>("MaterialData", frameIndex);
}

