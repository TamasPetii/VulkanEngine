#include "Instance.h"
#include "../Logger/Checker.h"
#include "ValidationLayer.h"
#include "DebugMessenger.h"

Vk::Instance::Instance(std::span<const char*> requiredExtensions)
{
	Init(requiredExtensions);
}

Vk::Instance::~Instance()
{
	Destroy();
}

VkInstance Vk::Instance::Value() const
{
	return instance;
}

void Vk::Instance::Init(std::span<const char*> requiredExtensions)
{
	CHECK_ERROR_NOTNULL(Vk::ValidationLayer::ValidationLayerEnabled() && !Vk::ValidationLayer::CheckValidationLayerSupport());

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Dendrite Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (Vk::ValidationLayer::ValidationLayerEnabled())
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(Vk::ValidationLayer::GetValidationLayers().size());
		createInfo.ppEnabledLayerNames = Vk::ValidationLayer::GetValidationLayers().data();

		Vk::DebugMessenger::CreateDebugMessengerInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	VK_CHECK_MESSAGE(vkCreateInstance(&createInfo, nullptr, &instance), "Failed to create vulkan instance!");
}

void Vk::Instance::Destroy()
{
	if(instance != VK_NULL_HANDLE)
		vkDestroyInstance(instance, nullptr);

	instance = VK_NULL_HANDLE;
}