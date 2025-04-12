#include "Engine.h"
#include <random>

Engine::Engine()
{
}

Engine::~Engine()
{
	Cleanup();
}

void Engine::Cleanup()
{
	frameTimer.reset();
	renderer.reset();
	registry.reset();
	resourceManager.reset();
	Vk::VulkanContext::DestroyContext();
}

void Engine::Initialize()
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	vulkanContext->Init();

	frameTimer = std::make_shared<Timer>();
	renderer = std::make_shared<Renderer>();
	resourceManager = std::make_shared<ResourceManager>();

	InitRegistry();
	InitSystems();
	InitComponentBufferManager();

	resourceManager->GetImageManager()->LoadImage("../Assets/Texture.jpg");
	resourceManager->GetModelManager()->LoadModel("../Assets/Mamut/Mamut.obj");
	resourceManager->GetModelManager()->LoadModel("../Assets/Sponza/Sponza.obj");
}

void Engine::SetRequiredWindowExtensions(std::span<const char*> extensionNames)
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	vulkanContext->SetRequiredInstanceExtensions(extensionNames);
}

void Engine::SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function)
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	vulkanContext->SetSurfaceCreationFunction(function);
}

void Engine::SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function)
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	vulkanContext->SetWindowExtentFunction(function);
}

void Engine::SetGuiRenderFunction(const std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)>& function)
{
	if (renderer)
		renderer->SetGuiRenderFunction(function);
}

void Engine::WindowResizeEvent()
{
	renderer->RecreateSwapChain();
}

void Engine::InitSystems()
{
	systems[Unique::typeID<TransformSystem>()] = std::make_shared<TransformSystem>();
	systems[Unique::typeID<CameraSystem>()] = std::make_shared<CameraSystem>();
	systems[Unique::typeID<MaterialSystem>()] = std::make_shared<MaterialSystem>();
}

void Engine::InitRegistry()
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

	for (uint32_t i = 0; i < 256; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent>(entity);

		auto transformComponent = registry->GetComponent<TransformComponent>(entity);
		transformComponent->rotation = 180.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->translation = 100.f * glm::vec3(dist(rng), dist(rng), dist(rng));
		transformComponent->scale = glm::vec3(0.01);
	}
}

void Engine::InitComponentBufferManager()
{
	Vk::BufferConfig config;
	config.size = 1;
	config.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
	config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	resourceManager->GetComponentBufferManager()->RegisterBuffer("CameraComponentGPU", config);
	resourceManager->GetComponentBufferManager()->RegisterBuffer("TransformComponentGPU", config);
	resourceManager->GetComponentBufferManager()->RegisterBuffer("MaterialComponentGPU", config);
}

void Engine::CheckForComponentBufferResize()
{
	resourceManager->GetComponentBufferManager()->RecreateBuffer<TransformComponentGPU>("TransformComponentGPU", registry->GetPool<TransformComponent>()->GetDenseSize(), framesInFlightIndex);
	resourceManager->GetComponentBufferManager()->RecreateBuffer<CameraComponentGPU>("CameraComponentGPU", registry->GetPool<CameraComponent>()->GetDenseSize(), framesInFlightIndex);
	//resourceManager->GetComponentBufferManager()->RecreateBuffer<MaterialComponentGPU>("MaterialComponentGPU", registry->GetPool<MaterialComponent>()->GetDenseSize(), framesInFlightIndex);
}

void Engine::Update()
{
	static float time = 0;
	static int counter = 0;

	frameTimer->Update();
	time += frameTimer->GetFrameDeltaTime();
	counter++;

	if (time > 1)
	{
		std::cout << std::format("Fps: {}", counter) << std::endl;
		time = 0;
		counter = 0;
	}

	auto viewPortSize = resourceManager->GetVulkanManager()->GetFrameDependentFrameBuffer("Main", framesInFlightIndex)->GetSize();
	auto cameraComponent = registry->GetComponent<CameraComponent>(0);
	if (viewPortSize.width != cameraComponent->width || viewPortSize.height != cameraComponent->height)
	{
		cameraComponent->width = viewPortSize.width;
		cameraComponent->height = viewPortSize.height;
		registry->GetPool<CameraComponent>()->GetBitset(0).set(UPDATE_BIT, true);
	}

	SystemUpdate(frameTimer->GetFrameDeltaTime());
	SystemFinish();

	InputManager::Instance()->UpdatePrevious();
}

void Engine::SystemUpdate(float deltaTime)
{
	{ //Transform System
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnUpdate(registry, deltaTime);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	{ //Camera System
		Timer timer;
		systems[Unique::typeID<CameraSystem>()]->OnUpdate(registry, deltaTime);
		systemTimes[Unique::typeID<CameraSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	{ //Material System
		Timer timer;
		systems[Unique::typeID<MaterialSystem>()]->OnUpdate(registry, deltaTime);
		systemTimes[Unique::typeID<MaterialSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	auto model = resourceManager->GetModelManager()->GetModel("../Assets/Sponza/Sponza.obj");
	model->ResetInstanceCount();
	for (uint32_t i = 1; i < 10; i++)
		model->AddIndex({ i, i, 0, 0 });
}

void Engine::SystemUpdateGPU()
{
	CheckForComponentBufferResize();

	{ //Transform System
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), framesInFlightIndex);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	{ //Camera System
		Timer timer;
		systems[Unique::typeID<CameraSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), framesInFlightIndex);
		systemTimes[Unique::typeID<CameraSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	{ //Material System
		Timer timer;
		systems[Unique::typeID<MaterialSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), framesInFlightIndex);
		systemTimes[Unique::typeID<MaterialSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	auto model = resourceManager->GetModelManager()->GetModel("../Assets/Sponza/Sponza.obj");
	model->UploadInstanceDataToGPU(framesInFlightIndex);
	model->UpdateIndirectCommandsInstanceCount(model->GetInstanceCount(), framesInFlightIndex);
}

void Engine::SystemFinish()
{
	{ //Transform System
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnFinish(registry);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	{ //Camera System
		Timer timer;
		systems[Unique::typeID<CameraSystem>()]->OnFinish(registry);
		systemTimes[Unique::typeID<CameraSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	{ //Material System
		Timer timer;
		systems[Unique::typeID<MaterialSystem>()]->OnFinish(registry);
		systemTimes[Unique::typeID<MaterialSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}
}

void Engine::Render()
{
	renderer->Render(registry, resourceManager, framesInFlightIndex);
}

void Engine::SimulateFrame()
{
	Update();

	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto inFlightFence = resourceManager->GetVulkanManager()->GetFrameDependentFence("InFlight", framesInFlightIndex);

	vkWaitForFences(device->Value(), 1, &inFlightFence->Value(), VK_TRUE, UINT64_MAX);
	vkResetFences(device->Value(), 1, &inFlightFence->Value());

	SystemUpdateGPU();
	Render();

	framesInFlightIndex = (framesInFlightIndex + 1) % Settings::FRAMES_IN_FLIGHT;
}
