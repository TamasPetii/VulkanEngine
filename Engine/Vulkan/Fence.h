#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"

namespace Vk
{
	class ENGINE_API Fence
	{
	public:
		static VkFenceCreateInfo BuildCreateInfo(VkBool32 signaled);

		Fence(VkBool32 signaled);
		~Fence();
		const VkFence& Value() const;
		void ResetFence();
	private:
		void Initialize(VkBool32 signaled);
		void Cleanup();
	private:
		VkFence fence = VK_NULL_HANDLE;
	};
}

