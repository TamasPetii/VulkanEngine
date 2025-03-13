#pragma once
#include "../EngineApi.h"
#include "../Logger/Checker.h"

#include "Instance.h"
#include "Surface.h"

#include <vulkan/vulkan.h>
#include <functional>
#include <optional>
#include <set>

namespace Vk
{
	struct ENGINE_API QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> computeFamily;
		std::optional<uint32_t> transferFamily;

		bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value(); }
	};

	class ENGINE_API PhysicalDevice
	{
	public:
		PhysicalDevice(const Vk::Instance* const instance, const Vk::Surface* const surface, std::span<const char*> deviceExtensions);
		~PhysicalDevice();
		VkPhysicalDevice Value() const;
		const QueueFamilyIndices& GetQueueFamilyIndices() const;
	private:
		void Init(std::span<const char*> deviceExtensions);
		void Destroy();
	private:
		bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, std::span<const char*> deviceExtensions);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice, std::span<const char*> deviceExtensions);
		bool CheckSwapChainSupport(VkPhysicalDevice physicalDevice);
		bool CheckAnisotropySupport(VkPhysicalDevice physicalDevice);
	private:
		const Vk::Instance* const instance;
		const Vk::Surface* const surface;
		QueueFamilyIndices queueFamilyIndices;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	};
}


