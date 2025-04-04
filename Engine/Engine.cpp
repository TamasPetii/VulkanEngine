#include "Engine.h"
#include <random>

void Engine::InitRegistry()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0, 1);

	for (uint32_t i = 0; i < 4096; ++i)
	{
		auto entity = registry->CreateEntity();
		registry->AddComponents<TransformComponent>(entity);

		auto [component] = registry->GetComponents<TransformComponent>(entity);
		component->scale = glm::vec3(dist(rng), dist(rng), dist(rng));
		component->rotation = glm::vec3(dist(rng), dist(rng), dist(rng));
		component->translation = glm::vec3(dist(rng), dist(rng), dist(rng));
	}
}


Engine::Engine() : 
	isWindowResized(false)
{
}

Engine::~Engine()
{
	Clean();
}

void Engine::Init()
{
	auto vulkanContext = Vk::VulkanContext::GetContext();
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	vulkanContext->SetRequiredDeviceExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	vulkanContext->Init();

	GeometryManager::GetManager();

	renderer = std::make_shared<Renderer>();
	frameTimer = std::make_shared<FrameTimer>();

	registry = std::make_shared<Registry<DEFAULT_MAX_COMPONENTS>>();
	InitRegistry();

	systems[Unique::typeID<TransformSystem>()] = std::make_shared<TransformSystem>();

	componentBufferManager = std::make_shared<ComponetBufferManager>();

	{
		Vk::BufferConfig config;
		config.size = 1;
		config.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		componentBufferManager->RegisterBuffer("TransformComponentGPU", config);
	}
}

void Engine::Clean()
{
	componentBufferManager.reset();
	renderer.reset();
	GeometryManager::Destroy();
	Vk::VulkanContext::DestroyContext();
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

void Engine::SetGuiRenderFunction(const std::function<void(VkCommandBuffer commandBuffer)>& function)
{
	if (renderer)
		renderer->SetGuiRenderFunction(function);
}

void Engine::Update()
{
	static float time = 0;
	static int counter = 0;

	frameTimer->Update();

	time += frameTimer->GetDeltaTime();
	counter++;

	if (time > 1)
	{
		std::cout << std::format("Fps: {}", counter) << std::endl;
		time = 0;
		counter = 0;
	}

	
}

void Engine::Render()
{
	renderer->Render(registry, componentBufferManager, systems);
}

void Engine::WindowResizeEvent()
{
	renderer->RecreateSwapChain();
}

