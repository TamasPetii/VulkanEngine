#pragma once
#include "../EngineApi.h"
#include "Instance.h"
#include <vulkan/vulkan.h>
#include <functional>
#include <span>

namespace Vk
{
	class ENGINE_API Surface
	{
	public:
		Surface(const Vk::Instance* const instance, const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& creationFunction);
		~Surface();
		VkSurfaceKHR Value() const;
	private:
		void Init(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& creationFunction);
		void Destroy();
	private:
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		const Vk::Instance* const instance;
	};
}
