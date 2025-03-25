#include "VulkanContext.h"

Vk::VulkanContext* Vk::VulkanContext::context = nullptr;
bool Vk::VulkanContext::initialized = false;

Vk::VulkanContext::~VulkanContext()
{
	Destory();
}

Vk::VulkanContext* Vk::VulkanContext::GetContext()
{
	if (context == nullptr)
		context = new VulkanContext();

	return context;
}

void Vk::VulkanContext::DestroyContext()
{
	if (context != nullptr)
	{
		delete context;
		context = nullptr;
	}
}

void Vk::VulkanContext::SetRequiredDeviceExtension(const char* extensionName)
{
	requiredDeviceExtensions.push_back(extensionName);
}

void Vk::VulkanContext::SetRequiredInstanceExtension(const char* extensionName)
{
	requiredInstanceExtensions.push_back(extensionName);
}

void Vk::VulkanContext::SetRequiredDeviceExtensions(std::span<const char*> extensionNames)
{
	for (auto extension : extensionNames)
		SetRequiredDeviceExtension(extension);
}

void Vk::VulkanContext::SetRequiredInstanceExtensions(std::span<const char*> extensionNames)
{
	for (auto extension : extensionNames)
		SetRequiredInstanceExtension(extension);
}

void Vk::VulkanContext::Init()
{
	if (initialized)
		return;

	if (windowExtentFunction && surfaceCreationFunction)
	{
		if (Vk::ValidationLayer::ValidationLayerEnabled())
			SetRequiredInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		instance = std::make_unique<Vk::Instance>(requiredInstanceExtensions);
		debugMessenger = std::make_unique<Vk::DebugMessenger>(instance.get());
		surface = std::make_unique<Vk::Surface>(instance.get(), surfaceCreationFunction);
		physicalDevice = std::make_unique<Vk::PhysicalDevice>(instance.get(), surface.get(), requiredDeviceExtensions);
		device = std::make_unique<Vk::Device>(physicalDevice.get(), requiredDeviceExtensions);
		swapChain = std::make_unique<Vk::SwapChain>(physicalDevice.get(), device.get(), surface.get(), windowExtentFunction);
		immediateQueue = std::make_shared<Vk::ImmediateQueue>(physicalDevice.get(), device.get());
		initialized = true;
	}
}

void Vk::VulkanContext::SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function)
{
	windowExtentFunction = function;
}

void Vk::VulkanContext::SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function)
{
	surfaceCreationFunction = function;
}

const Vk::Instance* const Vk::VulkanContext::GetInstance()
{
	return instance.get();
}

const Vk::PhysicalDevice* const Vk::VulkanContext::GetPhysicalDevice()
{
	return physicalDevice.get();
}

const Vk::Device* const Vk::VulkanContext::GetDevice()
{
	return device.get();
}

const Vk::Surface* const Vk::VulkanContext::GetSurface()
{
	return surface.get();
}

Vk::SwapChain* Vk::VulkanContext::GetSwapChain()
{
	return swapChain.get();
}

std::shared_ptr<Vk::ImmediateQueue> Vk::VulkanContext::GetImmediateQueue()
{
	return immediateQueue;
}

void Vk::VulkanContext::Destory()
{
	vkDeviceWaitIdle(device->Value());

	immediateQueue.reset();
	swapChain.reset();
	device.reset();
	physicalDevice.reset();
	surface.reset();
	debugMessenger.reset();
	instance.reset();
}
