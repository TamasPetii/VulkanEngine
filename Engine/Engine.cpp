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
}

void Engine::InitRegistry()
{
	registry = std::make_shared<Registry>();

	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	for (uint32_t i = 0; i < 256; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent>(entity);

		auto [component] = registry->GetComponents<TransformComponent>(entity);
		component->scale = glm::vec3(dist(rng), dist(rng), dist(rng));
		component->rotation = glm::vec3(dist(rng), dist(rng), dist(rng));
		component->translation = glm::vec3(dist(rng), dist(rng), dist(rng));
	}
}

void Engine::InitComponentBufferManager()
{
	{
		Vk::BufferConfig config;
		config.size = 1;
		config.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		resourceManager->GetComponentBufferManager()->RegisterBuffer("TransformComponentGPU", config);
	}
}

void Engine::CheckForComponentBufferResize()
{
	resourceManager->GetComponentBufferManager()->RecreateBuffer<TransformComponentGPU>("TransformComponentGPU", registry->GetPool<TransformComponent>()->GetDenseSize(), framesInFlightIndex);
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

	{ //Transform System
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnUpdate(registry);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
	}

	InputManager::Instance()->UpdatePrevious();
}

void Engine::RefreshGpuData()
{
	CheckForComponentBufferResize();

	{ //Transform System
		Timer timer;
		systems[Unique::typeID<TransformSystem>()]->OnUploadToGpu(registry, resourceManager->GetComponentBufferManager(), framesInFlightIndex);
		systemTimes[Unique::typeID<TransformSystem>()] += timer.GetElapsedTime<std::chrono::milliseconds>();
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

	RefreshGpuData();
	Render();

	framesInFlightIndex = (framesInFlightIndex + 1) % framesInFlight;
}
