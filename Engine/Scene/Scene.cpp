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

void Scene::Update(std::shared_ptr<Timer> frameTimer, uint32_t frameIndex)
{
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

	auto geometry = resourceManager->GetGeometryManager()->GetShape("Cube");
	geometry->ResetInstanceCount();

	for (uint32_t i = 0; i < 32768; i++)
		geometry->AddIndex({ i + 1, i + 1, i, 0 });

	UpdateSystems(frameTimer->GetFrameDeltaTime());
	FinishSystems();
}

void Scene::UpdateGPU(uint32_t frameIndex)
{
	auto geometry = resourceManager->GetGeometryManager()->GetShape("Cube");
	geometry->UploadInstanceDataToGPU(frameIndex);

	UpdateSystemsGPU(frameIndex);
}

void Scene::Initialize()
{
	InitializeSystems();
	InitializeRegistry();
}

void Scene::Cleanup()
{
	registry.reset();
	systems.clear();
}


void Scene::InitializeSystems()
{
	systems[Unique::typeID<TransformSystem>()] = std::make_shared<TransformSystem>();
	systems[Unique::typeID<CameraSystem>()] = std::make_shared<CameraSystem>();
	systems[Unique::typeID<MaterialSystem>()] = std::make_shared<MaterialSystem>();
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

	for (uint32_t i = 0; i < 32768; ++i)
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

void Scene::UpdateSystems(float deltaTime)
{
	{ //Transform System
		Timer timer{};
		systems[Unique::typeID<TransformSystem>()]->OnUpdate(registry, deltaTime);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime();
	}

	{ //Camera System
		Timer timer;
		systems[Unique::typeID<CameraSystem>()]->OnUpdate(registry, deltaTime);
		systemTimes[Unique::typeID<CameraSystem>()] += timer.GetElapsedTime();
	}

	{ //Material System
		Timer timer;
		systems[Unique::typeID<MaterialSystem>()]->OnUpdate(registry, deltaTime);
		systemTimes[Unique::typeID<MaterialSystem>()] += timer.GetElapsedTime();
	}
}

void Scene::FinishSystems()
{
	{ //Transform System Finish
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnFinish(registry);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime();
	}

	{ //Camera System Finish
		Timer timer;
		systems[Unique::typeID<CameraSystem>()]->OnFinish(registry);
		systemTimes[Unique::typeID<CameraSystem>()] += timer.GetElapsedTime();
	}

	{ //Material System Finish
		Timer timer;
		systems[Unique::typeID<MaterialSystem>()]->OnFinish(registry);
		systemTimes[Unique::typeID<MaterialSystem>()] += timer.GetElapsedTime();
	}
}

void Scene::UpdateSystemsGPU(uint32_t frameIndex)
{
	{ //Transform System
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), frameIndex);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime();
	}

	{ //Camera System
		Timer timer;
		systems[Unique::typeID<CameraSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), frameIndex);
		systemTimes[Unique::typeID<CameraSystem>()] += timer.GetElapsedTime();
	}

	{ //Material System
		Timer timer;
		systems[Unique::typeID<MaterialSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), frameIndex);
		systemTimes[Unique::typeID<MaterialSystem>()] += timer.GetElapsedTime();
	}

}

