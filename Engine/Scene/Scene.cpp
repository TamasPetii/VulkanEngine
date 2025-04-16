#include "Scene.h"
#include <random>
#include <future>
#include <thread>

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
			transformPool->GetBitset(entity).set(UPDATE_BIT, true);
		}
	);

	/*
	auto materialPool = registry->GetPool<MaterialComponent>();
	std::for_each(std::execution::par,
		materialPool->GetDenseEntities().begin(),
		materialPool->GetDenseEntities().end(),
		[&](Entity entity) -> void
		{
			auto materialComponent = materialPool->GetComponent(entity);
			materialComponent->color = glm::vec4(glm::vec3(glm::sin(frameTimer->GetFrameElapsedTime() * 2 * glm::pi<float>()) + 2), 1);
			materialPool->GetBitset(entity).set(UPDATE_BIT, true);
		}
	);
	*/

	//Update Camera Size
	auto viewPortSize = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex)->GetSize();
	auto cameraComponent = registry->GetComponent<CameraComponent>(0);
	if (viewPortSize.width != cameraComponent->width || viewPortSize.height != cameraComponent->height)
	{
		cameraComponent->width = viewPortSize.width;
		cameraComponent->height = viewPortSize.height;
		registry->GetPool<CameraComponent>()->GetBitset(0).set(UPDATE_BIT, true);
	}

	/*
	auto geometry = resourceManager->GetGeometryManager()->GetShape("Cube");
	geometry->ResetInstanceCount();

	auto shapePool = registry->GetPool<ShapeComponent>();
	std::for_each(std::execution::seq, shapePool->GetDenseEntities().begin(), shapePool->GetDenseEntities().end(),[&](Entity entity) -> void {
		auto shapeIndex = shapePool->GetIndex(entity);
		auto shapeComponent = shapePool->GetComponent(entity);

		if (shapeComponent->shape)
			shapeComponent->shape->AddIndex(shapeIndex);
		}
	);
	*/

	auto model = resourceManager->GetModelManager()->GetModel("../Assets/Sponza/Sponza.obj");
	model->ResetInstanceCount();

	auto modelPool = registry->GetPool<ModelComponent>();
	std::for_each(std::execution::seq, modelPool->GetDenseEntities().begin(), modelPool->GetDenseEntities().end(), [&](Entity entity) -> void {
		auto modelIndex = modelPool->GetIndex(entity);
		auto modelComponent = modelPool->GetComponent(entity);

		if (modelComponent->model)
			modelComponent->model->AddIndex(modelIndex);
		}
	);

	UpdateSystems(frameTimer->GetFrameDeltaTime());
	FinishSystems();
}

void Scene::UpdateGPU(uint32_t frameIndex)
{
	auto model = resourceManager->GetModelManager()->GetModel("../Assets/Sponza/Sponza.obj");
	model->UploadInstanceDataToGPU(frameIndex);

	/*
	auto geometry = resourceManager->GetGeometryManager()->GetShape("Cube");
	geometry->UploadInstanceDataToGPU(frameIndex);
	*/

	UpdateComponentBuffers(frameIndex);
	UpdateSystemsGPU(frameIndex);
}

void Scene::InitializeRegistry()
{
	registry = std::make_shared<Registry>();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	resourceManager->GetImageManager()->LoadImage("../Assets/Texture.jpg");

	{ //Camera
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent>(entity);
		registry->AddComponents<CameraComponent>(entity);
	}

	/*
	for (uint32_t i = 0; i < 1000; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, MaterialComponent, ShapeComponent>(entity);

		auto [transformComponent, materialComponent, shapeComponent] = registry->GetComponents<TransformComponent, MaterialComponent, ShapeComponent>(entity);
		transformComponent->rotation = 180.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->translation = 100.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->scale = glm::vec3(1);

		materialComponent->color = glm::vec4(dist(rng), dist(rng), dist(rng), 1);
		materialComponent->albedo = resourceManager->GetImageManager()->LoadImage("../Assets/Texture.jpg");

		shapeComponent->shape = resourceManager->GetGeometryManager()->GetShape("Cube");
	}
	*/

	for (uint32_t i = 0; i < 20; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, ModelComponent>(entity);

		auto [transformComponent, modelComponent] = registry->GetComponents<TransformComponent, ModelComponent>(entity);
		transformComponent->rotation = 180.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->translation = 100.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->scale = glm::vec3(1);

		modelComponent->model = resourceManager->GetModelManager()->LoadModel("../Assets/Sponza/Sponza.obj");
	}
}

void Scene::InitializeSystems()
{
	InitSystem<TransformSystem>();
	InitSystem<CameraSystem>();
	InitSystem<MaterialSystem>();
	InitSystem<ShapeSystem>();
	InitSystem<ModelSystem>();
}

void Scene::UpdateSystems(float deltaTime)
{
	Timer timer{};

	std::unordered_map<std::type_index, std::future<void>> futures;

	auto LaunchSystemUpdateAsync = [&]<typename T>() -> void {
		futures[Unique::typeID<T>()] = std::async(std::launch::async, 
			[&]() -> void {
				UpdateSystem<T>(deltaTime);
			});
	};

	LaunchSystemUpdateAsync.template operator() < TransformSystem > ();
	LaunchSystemUpdateAsync.template operator() < CameraSystem > ();
	LaunchSystemUpdateAsync.template operator() < MaterialSystem > ();
	LaunchSystemUpdateAsync.template operator() < ShapeSystem > ();
	LaunchSystemUpdateAsync.template operator() < ModelSystem > ();

	for (auto& [_, future] : futures) {
		future.get();
	}

	resourceManager->GetBenchmarkManager()->AddBenchmarkTime<System>(timer.GetElapsedTime());
}

void Scene::FinishSystems()
{
	Timer timer{};

	std::unordered_map<std::type_index, std::future<void>> futures;

	auto LaunchSystemFinishAsync = [&]<typename T>() -> void {
		futures[Unique::typeID<T>()] = std::async(std::launch::async,
			[&]() -> void {
				FinishSystem<T>();
			});
	};

	LaunchSystemFinishAsync.template operator() < TransformSystem > ();
	LaunchSystemFinishAsync.template operator() < CameraSystem > ();
	LaunchSystemFinishAsync.template operator() < MaterialSystem > ();
	LaunchSystemFinishAsync.template operator() < ShapeSystem > ();
	LaunchSystemFinishAsync.template operator() < ModelSystem > ();

	for (auto& [_, future] : futures) {
		future.get();
	}

	resourceManager->GetBenchmarkManager()->AddBenchmarkTime<System>(timer.GetElapsedTime());
}

void Scene::UpdateSystemsGPU(uint32_t frameIndex)
{
	Timer timer{};

	std::unordered_map<std::type_index, std::future<void>> futures;

	auto LaunchSystemUpdateGpuAsync = [&]<typename T>() -> void {
		futures[Unique::typeID<T>()] = std::async(std::launch::async,
			[&]() -> void {
				UpdateGpuSystem<T>(frameIndex);
			});
	};

	LaunchSystemUpdateGpuAsync.template operator() < TransformSystem > ();
	LaunchSystemUpdateGpuAsync.template operator() < CameraSystem > ();
	LaunchSystemUpdateGpuAsync.template operator() < MaterialSystem > ();
	LaunchSystemUpdateGpuAsync.template operator() < ShapeSystem > ();
	LaunchSystemUpdateGpuAsync.template operator() < ModelSystem > ();

	for (auto& [_, future] : futures) {
		future.get();
	}

	resourceManager->GetBenchmarkManager()->AddBenchmarkTime<System>(timer.GetElapsedTime());
}

void Scene::UpdateComponentBuffers(uint32_t frameIndex)
{
	//TODO: Error when size 0 
	RecalculateGpuBufferSize<TransformComponent, TransformComponentGPU>("TransformData", frameIndex);
	RecalculateGpuBufferSize<CameraComponent, CameraComponentGPU>("CameraData", frameIndex);
	//RecalculateGpuBufferSize<MaterialComponent, MaterialComponentGPU>("MaterialData", frameIndex);
	//RecalculateGpuBufferSize<ShapeComponent, RenderIndicesGPU>("ShapeRenderIndicesData", frameIndex);
	RecalculateGpuBufferSize<ModelComponent, RenderIndicesGPU>("ModelRenderIndicesData", frameIndex);
}

