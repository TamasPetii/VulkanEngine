#pragma once
#include "Device.h"
#include "PhysicalDevice.h"
#include <memory>
#include <functional>
#include <mutex>

namespace Vk
{
	class ENGINE_API ImmediateQueue
	{
	public:
		ImmediateQueue(const PhysicalDevice* const physicalDevice, const Device* const device);
		~ImmediateQueue();
		void Submit(const std::function<void(VkCommandBuffer)>& function);
	private:
		void Initialize();
		void Cleanup();
	private:
		std::mutex submitMutex;
		const Device* const device;
		const PhysicalDevice* const physicalDevice;
		VkFence immediateFence = VK_NULL_HANDLE;
		VkCommandPool immediatePool = VK_NULL_HANDLE;
		VkCommandBuffer immediateBuffer = VK_NULL_HANDLE;
	};
}
