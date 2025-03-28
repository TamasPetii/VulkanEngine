#include "Engine.h"

Engine::Engine() : 
	isWindowResized(false)
{
}

Engine::~Engine()
{
	Clean();
}

class A {};
class B {};
class C {};

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

	registry = std::make_shared<Registry<32>>();
	registry->RegisterEntitiesWithBitset<A, C>();
	registry->RegisterEntitiesWithBitset<B>();
	registry->RegisterEntitiesWithBitset<A, B, C>();

	auto entity1 = registry->CreateEntity();
	auto entity2 = registry->CreateEntity();
	auto entity3 = registry->CreateEntity();

	registry->AddComponents<A, B, C>(entity1);
	registry->AddComponents<A, B>(entity2);
	registry->AddComponents<B, C>(entity3);

	auto [a1, c1] = registry->GetComponents<A, C>(entity1);
	auto [a2, b2] = registry->GetComponents<A, B>(entity2);
	auto [a3, b3, c3] = registry->GetComponents<A, C, B>(entity3);

	registry->RemoveComponents<C>(entity1);
	registry->DestroyEntity(entity2);
}

void Engine::Clean()
{
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
	renderer->Render();
}

void Engine::WindowResizeEvent()
{
	renderer->RecreateSwapChain();
}

