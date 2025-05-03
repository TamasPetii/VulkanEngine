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

void Scene::InitializeRegistry()
{
	registry = std::make_shared<Registry>();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	/*
	std::string imagePath = "../Assets/Texture.jpg";
	resourceManager->GetImageManager()->LoadImage(imagePath, false);
	resourceManager->GetImageManager()->WaitForImageFuture(imagePath);
	*/

	{ //Camera
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, CameraComponent>(entity);
		registry->GetComponent<CameraComponent>(entity).isMain = true;
	}

	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, ModelComponent, DefaultColliderComponent>(entity);
		auto [transformComponent, modelComponent] = registry->GetComponents<TransformComponent, ModelComponent>(entity);
		modelComponent.model = resourceManager->GetModelManager()->LoadModel("C:/Users/User/Desktop/Bistro_v5_2/BistroExterior.fbx");
		modelComponent.hasDirectxNormals = true;
	}

	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, ModelComponent, DefaultColliderComponent>(entity);
		auto [transformComponent, modelComponent] = registry->GetComponents<TransformComponent, ModelComponent>(entity);
		modelComponent.model = resourceManager->GetModelManager()->LoadModel("C:/Users/User/Desktop/Bistro_v5_2/BistroInterior.fbx");
		modelComponent.hasDirectxNormals = true;
	}
	
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, ModelComponent, DefaultColliderComponent>(entity);
		auto [transformComponent, modelComponent] = registry->GetComponents<TransformComponent, ModelComponent>(entity);
		transformComponent.scale = glm::vec3(0.01);
		modelComponent.model = resourceManager->GetModelManager()->LoadModel("C:/Users/User/Desktop/VulkanEngine/Assets/Sponza/sponza.obj");
	}

	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, ModelComponent, DefaultColliderComponent>(entity);
		auto [transformComponent, modelComponent] = registry->GetComponents<TransformComponent, ModelComponent>(entity);
		transformComponent.scale = glm::vec3(500);
		modelComponent.model = resourceManager->GetModelManager()->LoadModel("C:/Users/User/Desktop/VulkanEngine/Assets/Man/Walking.dae");
	}
	
	std::array<std::string, 5> shapes = { "Cube", "Sphere", "Cone", "Pyramid", "Cylinder" };
	std::uniform_int_distribution<size_t> shapeDist(0, shapes.size() - 1); // for shape selection

	for (uint32_t i = 0; i < 1000; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent, MaterialComponent, ShapeComponent, DefaultColliderComponent>(entity);

		auto [transformComponent, materialComponent, shapeComponent] = registry->GetComponents<TransformComponent, MaterialComponent, ShapeComponent>(entity);
		transformComponent.rotation = 180.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent.translation = 100.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent.scale = glm::vec3(1);

		materialComponent.color = glm::vec4(dist(rng), dist(rng), dist(rng), 1);
		//materialComponent.albedo = resourceManager->GetImageManager()->LoadImage("../Assets/Texture.jpg");

		shapeComponent.shape = resourceManager->GetGeometryManager()->GetShape(shapes[shapeDist(rng)]);
	}
}

void Scene::Update(std::shared_ptr<Timer> frameTimer, uint32_t frameIndex)
{
	/*
	//Update Registry Transforms
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	if (auto transformPool = registry->GetPool<TransformComponent>())
	{
		std::for_each(std::execution::par,
			transformPool->GetDenseIndices().begin(),
			transformPool->GetDenseIndices().end(),
			[&](Entity entity) -> void
			{
				auto transformComponent = transformPool->GetData(entity);
				transformComponent->scale = glm::vec3(glm::sin(frameTimer->GetFrameElapsedTime() * 2 * glm::pi<float>()) + 2);
				transformPool->SetBit<UPDATE_BIT>(entity);
			}
		);
	}
	*/

	//Update Camera Size
	auto viewPortSize = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", frameIndex)->GetSize();
	auto& cameraComponent = registry->GetComponent<CameraComponent>(0);
	if (viewPortSize.width != cameraComponent.width || viewPortSize.height != cameraComponent.height)
	{
		cameraComponent.width = viewPortSize.width;
		cameraComponent.height = viewPortSize.height;
		registry->GetPool<CameraComponent>()->GetBitset(0).set(UPDATE_BIT, true);
	}

	UpdateSystems(frameTimer->GetFrameDeltaTime());
	FinishSystems();
}

void Scene::UpdateGPU(uint32_t frameIndex)
{
	UpdateComponentBuffers(frameIndex);
	UpdateSystemsGPU(frameIndex);
}

void Scene::InitializeSystems()
{
	InitSystem<TransformSystem>();
	InitSystem<CameraSystem>();
	InitSystem<MaterialSystem>();
	InitSystem<ShapeSystem>();
	InitSystem<ModelSystem>();
	InitSystem<InstanceSystem>();
	InitSystem<DefaultColliderSystem>();
	InitSystem<FrustumCullingSystem>();
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

	//DefaultColliderSystem uses these systems output as input
	futures[Unique::typeID<TransformSystem>()].get();
	futures[Unique::typeID<ShapeSystem>()].get();
	futures[Unique::typeID<ModelSystem>()].get();
	LaunchSystemUpdateAsync.template operator() < DefaultColliderSystem > ();

	//FrustumCullingSystem uses these systems output as input
	futures[Unique::typeID<CameraSystem>()].get();
	futures[Unique::typeID<DefaultColliderSystem>()].get();
	LaunchSystemUpdateAsync.template operator() < FrustumCullingSystem > ();

	//InstanceSystem uses these systems output as input
	futures[Unique::typeID<FrustumCullingSystem>()].get();
	LaunchSystemUpdateAsync.template operator() < InstanceSystem > ();

	for (auto& [_, future] : futures) {
		if(future.valid())
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
	LaunchSystemFinishAsync.template operator() < InstanceSystem > ();
	LaunchSystemFinishAsync.template operator() < DefaultColliderSystem > ();

	for (auto& [_, future] : futures) {
		if (future.valid())
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
	LaunchSystemUpdateGpuAsync.template operator() < InstanceSystem > ();
	LaunchSystemUpdateGpuAsync.template operator() < DefaultColliderSystem > ();

	for (auto& [_, future] : futures) {
		if (future.valid())
			future.get();
	}

	resourceManager->GetBenchmarkManager()->AddBenchmarkTime<System>(timer.GetElapsedTime());
}

void Scene::UpdateComponentBuffers(uint32_t frameIndex)
{
	RecalculateGpuBufferSize<TransformComponent, TransformComponentGPU>("TransformData", frameIndex);
	RecalculateGpuBufferSize<CameraComponent, CameraComponentGPU>("CameraData", frameIndex);
	RecalculateGpuBufferSize<MaterialComponent, MaterialComponentGPU>("MaterialData", frameIndex);
	RecalculateGpuBufferSize<ShapeComponent, RenderIndicesGPU>("ShapeRenderIndicesData", frameIndex);
	RecalculateGpuBufferSize<ModelComponent, RenderIndicesGPU>("ModelRenderIndicesData", frameIndex);
	RecalculateGpuBufferSize<DefaultColliderComponent, glm::mat4>("DefaultColliderAabbData", frameIndex);
	RecalculateGpuBufferSize<DefaultColliderComponent, glm::mat4>("DefaultColliderObbData", frameIndex);
	RecalculateGpuBufferSize<DefaultColliderComponent, glm::mat4>("DefaultColliderSphereData", frameIndex);
}