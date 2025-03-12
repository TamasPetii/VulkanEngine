#pragma once
#include "../EngineApi.h"
#include <vulkan/vulkan.h>
#include <functional>
#include <span>

namespace Vk
{
	class ENGINE_API Instance
	{
	public:
		Instance(std::span<const char*> requiredExtensions);
		~Instance();
		VkInstance Value() const;
	private:
		void Init(std::span<const char*> requiredExtensions);
		void Destroy();
	private:
		VkInstance instance = VK_NULL_HANDLE;
	};
}


