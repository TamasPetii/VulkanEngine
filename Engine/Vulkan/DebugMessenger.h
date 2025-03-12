#pragma once
#include "../EngineApi.h"
#include "Instance.h"
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <span>

namespace Vk
{
	class ENGINE_API DebugMessenger
	{
	public:
		DebugMessenger(const Vk::Instance* const instance);
		~DebugMessenger();
		static void CreateDebugMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	private:
		void Init();
		void Destroy();
	private:
		const Vk::Instance* const instance;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	};
}


