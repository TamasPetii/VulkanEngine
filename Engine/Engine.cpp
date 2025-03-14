#include "Engine.h"

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

	frameTimer = std::make_unique<FrameTimer>();
	registry = std::make_shared<Registry>();

	std::shared_ptr<Vk::ShaderModule> vertexShader = std::make_shared<Vk::ShaderModule>("../VulkanEngine/Shaders/Shader.vert", VK_SHADER_STAGE_VERTEX_BIT);
	std::shared_ptr<Vk::ShaderModule> fragmentShader = std::make_shared<Vk::ShaderModule>("../VulkanEngine/Shaders/Shader.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
}

void Engine::Clean()
{
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
}

void Engine::WindowResizeEvent()
{
	isWindowResized = true;
	Vk::VulkanContext::GetContext()->GetSwapChain()->ReCreate();
	isWindowResized = false;
}

