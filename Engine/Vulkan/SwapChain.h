#pragma once
#include "../EngineApi.h"
#include "Device.h"

#include <vulkan/vulkan.h>
#include <functional>
#include <algorithm>
#include <span>

namespace Vk
{
	struct ENGINE_API SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class ENGINE_API SwapChain
	{
	public:
		SwapChain(const Vk::PhysicalDevice* const physicalDevice, const Vk::Device* const device, const Vk::Surface* const surface, const std::function<std::pair<int, int>()>& windowExtentFunction);
		~SwapChain();

		void Reset();
		VkSwapchainKHR Value() const;
		VkExtent2D GetExtent() const;
		VkFormat GetImageFormat() const;
	private:
		void Init();
		void Destroy();
	private:
		SwapChainSupportDetails QuerySwapChainSupportDetails();
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
		VkPresentModeKHR ChooseSwapPresentMode();
		VkExtent2D ChooseSwapExtent();
	private:
		const Vk::Device* const device;
		const Vk::Surface* const surface;
		const Vk::PhysicalDevice* const physicalDevice;
		VkSwapchainKHR swapChain = VK_NULL_HANDLE;

		VkExtent2D swapChainExtent;
		VkFormat swapChainImageFormat;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		SwapChainSupportDetails swapChainSupportDetails;
		std::function<std::pair<int, int>()> windowExtentFunction;
	};
}


