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
	scene = std::make_shared<Scene>(resourceManager);

	resourceManager->GetImageManager()->LoadImage("../Assets/Texture.jpg");
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

	scene->Update(frameTimer, frameIndex);
	InputManager::Instance()->UpdatePrevious();
}

void Engine::UpdateGPU()
{
	scene->UpdateGPU(frameIndex);
}

void Engine::Render()
{
	renderer->Render(scene->GetRegistry(), resourceManager, frameIndex);
}

void Engine::SimulateFrame()
{
	Update();

	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto inFlightFence = resourceManager->GetVulkanManager()->GetFrameDependentFence("InFlight", frameIndex);

	vkWaitForFences(device->Value(), 1, &inFlightFence->Value(), VK_TRUE, UINT64_MAX);
	vkResetFences(device->Value(), 1, &inFlightFence->Value());

	UpdateGPU();
	Render();

	frameIndex = (frameIndex + 1) % Settings::FRAMES_IN_FLIGHT;
}
