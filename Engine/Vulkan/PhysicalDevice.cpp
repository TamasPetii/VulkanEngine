#include "PhysicalDevice.h"

Vk::PhysicalDevice::PhysicalDevice(const Vk::Instance* const instance, const Vk::Surface* const surface, std::span<const char*> deviceExtensions) :
	instance(instance),
	surface(surface)
{
	Init(deviceExtensions);
}

Vk::PhysicalDevice::~PhysicalDevice()
{
	Destroy();
}

VkPhysicalDevice Vk::PhysicalDevice::Value() const
{
	return physicalDevice;
}

const Vk::QueueFamilyIndices& Vk::PhysicalDevice::GetQueueFamilyIndices() const
{
	return queueFamilyIndices;
}

void Vk::PhysicalDevice::Init(std::span<const char*> deviceExtensions)
{
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance->Value(), &physicalDeviceCount, nullptr);

	CHECK_ERROR_NULL_MESSAGE(physicalDeviceCount, "Failed to find GPUs with vulkan support!");
		
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance->Value(), &physicalDeviceCount, physicalDevices.data());

	for (auto& physicalDevice : physicalDevices)
	{
		if (IsDeviceSuitable(physicalDevice, deviceExtensions))
		{
			this->physicalDevice = physicalDevice;
			this->queueFamilyIndices = FindQueueFamilies(physicalDevice);
			break;
		}
	}

	CHECK_ERROR_NULL_MESSAGE(this->physicalDevice, "Failed to find a suitable GPU!");
}

void Vk::PhysicalDevice::Destroy()
{
}

bool Vk::PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice, std::span<const char*> deviceExtensions)
{
	return FindQueueFamilies(physicalDevice).IsComplete() &&
		CheckDeviceExtensionSupport(physicalDevice, deviceExtensions) &&
		CheckSwapChainSupport(physicalDevice) &&
		CheckAnisotropySupport(physicalDevice);
}

Vk::QueueFamilyIndices Vk::PhysicalDevice::FindQueueFamilies(VkPhysicalDevice physicalDevice)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilies.size(); i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			indices.computeFamily = i;
		}

		if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			indices.transferFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface->Value(), &presentSupport);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}
	}

	return indices;
}

bool Vk::PhysicalDevice::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, std::span<const char*> deviceExtensions)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool Vk::PhysicalDevice::CheckSwapChainSupport(VkPhysicalDevice physicalDevice)
{
	//Surface Formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface->Value(), &formatCount, nullptr);

	//Surface Present Modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface->Value(), &presentModeCount, nullptr);

	return formatCount > 0 && presentModeCount > 0;
}

bool Vk::PhysicalDevice::CheckAnisotropySupport(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

	return supportedFeatures.samplerAnisotropy;
}
