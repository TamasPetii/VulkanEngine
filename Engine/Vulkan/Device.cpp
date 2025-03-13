#include "Device.h"

Vk::Device::Device(const Vk::PhysicalDevice* const physicalDevice, std::span<const char*> deviceExtensions) :
	physicalDevice(physicalDevice)
{
	Init(deviceExtensions);
}

Vk::Device::~Device()
{
	Destroy();
}

VkDevice Vk::Device::Value() const
{
	return device;
}

VkQueue Vk::Device::GetQueue(QueueType type) const
{
	switch (type)
	{
	case QueueType::GRAPHICS: return graphicsQueue; break;
	case QueueType::PRESENTATION: return presentQueue; break;
	case QueueType::COMPUTE: return computeQueue; break;
	case QueueType::TRANSFER: return transferQueue; break;
	default:
		CHECK_ERROR_NULL_MESSAGE(0, "QueueType not supported!");
	}
}

void Vk::Device::Init(std::span<const char*> deviceExtensions)
{
	QueueFamilyIndices indices = physicalDevice->GetQueueFamilyIndices();

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.computeFamily.value(), indices.transferFamily.value(), indices.presentFamily.value() };

	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkPhysicalDeviceVulkan12Features deviceFeatures12{};
	deviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	deviceFeatures12.descriptorIndexing = VK_TRUE;
	deviceFeatures12.bufferDeviceAddress = VK_TRUE;

	VkPhysicalDeviceVulkan13Features deviceFeatures13 = {};
	deviceFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	deviceFeatures13.dynamicRendering = VK_TRUE;
	deviceFeatures13.synchronization2 = VK_TRUE;
	deviceFeatures13.pNext = &deviceFeatures12;

	VkPhysicalDeviceFeatures2 deviceFeatures2 = {};
	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures2.pNext = &deviceFeatures13;
	deviceFeatures2.features = deviceFeatures;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.pNext = &deviceFeatures2;

	if (ValidationLayer::ValidationLayerEnabled())
	{
		auto validationLayers = ValidationLayer::GetValidationLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	VK_CHECK_MESSAGE(vkCreateDevice(physicalDevice->Value(), &createInfo, nullptr, &device), "Failed to create logical device!");

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	vkGetDeviceQueue(device, indices.computeFamily.value(), 0, &computeQueue);
	vkGetDeviceQueue(device, indices.transferFamily.value(), 0, &transferQueue);
}

void Vk::Device::Destroy()
{
	vkDestroyDevice(device, nullptr);
}
