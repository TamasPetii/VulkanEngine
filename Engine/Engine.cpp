#include "Engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
	Clean();
}

void Engine::Init()
{
	frameTimer = std::make_unique<FrameTimer>();

	SetRequiredDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	SetRequiredDeviceExtension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	SetRequiredDeviceExtension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	SetRequiredDeviceExtension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);

	if (Vk::ValidationLayer::ValidationLayerEnabled())
		SetRequiredInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	instance = std::make_unique<Vk::Instance>(requiredInstanceExtensions);
	debugMessenger = std::make_unique<Vk::DebugMessenger>(instance.get());
	surface = std::make_unique<Vk::Surface>(instance.get(), surfaceCreationFunction);
	physicalDevice = std::make_unique<Vk::PhysicalDevice>(instance.get(), surface.get(), requiredDeviceExtensions);
	device = std::make_unique<Vk::Device>(physicalDevice.get(), requiredDeviceExtensions);
	swapChain = std::make_unique<Vk::SwapChain>(physicalDevice.get(), device.get(), surface.get(), windowExtentFunction);
}

void Engine::Clean()
{
	vkDeviceWaitIdle(device->Value());

	swapChain.reset();
	device.reset();
	physicalDevice.reset();
	surface.reset();
	debugMessenger.reset();
	instance.reset();
}

void Engine::SetRequiredDeviceExtension(const char* extensionName)
{
	requiredDeviceExtensions.push_back(extensionName);
}

void Engine::SetRequiredInstanceExtension(const char* extensionName)
{
	requiredInstanceExtensions.push_back(extensionName);
}

void Engine::SetRequiredDeviceExtensions(std::span<const char*> extensionNames)
{
	for (auto extension : extensionNames)
		SetRequiredDeviceExtension(extension);
}

void Engine::SetRequiredInstanceExtensions(std::span<const char*> extensionNames)
{
	for (auto extension : extensionNames)
		SetRequiredInstanceExtension(extension);
}

void Engine::SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function)
{
	surfaceCreationFunction = function;
}

void Engine::SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function)
{
	windowExtentFunction = function;
}

void Engine::Update()
{
	frameTimer->Update();
}

void Engine::Render()
{
}

