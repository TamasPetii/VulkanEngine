#pragma once
#include "Device.h"
#include "PhysicalDevice.h"
#include <memory>
#include <mutex>
#include <functional>

namespace Vk
{
	class ENGINE_API ImmediateQueue
	{
	public:
		ImmediateQueue(const PhysicalDevice* const physicalDevice, const Device* const device);
		~ImmediateQueue();
		void SubmitGraphics(const std::function<void(VkCommandBuffer)>& function);
		void SubmitGraphics(std::span<VkCommandBufferSubmitInfo> commandBufferSubmitInfos);

		void SubmitTransfer(const std::function<void(VkCommandBuffer)>& function);
	private:
		void Initialize();
		void Cleanup();

		void InitResource(VkFence& fence, VkCommandPool& commandPool, VkCommandBuffer& commandBuffer, uint32_t queueFamilyIndex);
	private:
		
		const Device* const device;
		const PhysicalDevice* const physicalDevice;

		std::mutex graphicsMutex;
		VkFence graphicsFence = VK_NULL_HANDLE;
		VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;
		VkCommandBuffer graphicsCommandBuffer = VK_NULL_HANDLE;

		std::mutex transferMutex;
		VkFence transferFence = VK_NULL_HANDLE;
		VkCommandPool transferCommandPool = VK_NULL_HANDLE;
		VkCommandBuffer transferCommandBuffer = VK_NULL_HANDLE;
	};
}
