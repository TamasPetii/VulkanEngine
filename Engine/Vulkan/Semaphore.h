#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"

namespace Vk
{
	class ENGINE_API Semaphore
	{
	public:
		static VkSemaphoreCreateInfo BuildCreateInfo();

		Semaphore();
		~Semaphore();
		const VkSemaphore& Value() const;
	private:
		void Initialize();
		void Cleanup();
	private:
		VkSemaphore semaphore = VK_NULL_HANDLE;
	};
}

