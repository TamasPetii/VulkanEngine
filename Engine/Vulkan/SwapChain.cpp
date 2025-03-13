#include "SwapChain.h"

Vk::SwapChain::SwapChain(const Vk::PhysicalDevice* const physicalDevice, const Vk::Device* const device, const Vk::Surface* const surface, const std::function<std::pair<int, int>()>& windowExtentFunction) :
	physicalDevice(physicalDevice),
	device(device),
	surface(surface),
	windowExtentFunction(windowExtentFunction)
{
}

Vk::SwapChain::~SwapChain()
{
}

void Vk::SwapChain::Reset()
{
	vkDeviceWaitIdle(device->Value());
	Destroy();
	Init();
}

VkSwapchainKHR Vk::SwapChain::Value() const
{
	return swapChain;
}

VkExtent2D Vk::SwapChain::GetExtent() const
{
	return swapChainExtent;
}

VkFormat Vk::SwapChain::GetImageFormat() const
{
	return swapChainImageFormat;
}

void Vk::SwapChain::Init()
{
	swapChainSupportDetails = QuerySwapChainSupportDetails();
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat();
	VkPresentModeKHR presentMode = ChooseSwapPresentMode();
	VkExtent2D extent = ChooseSwapExtent();

	uint32_t imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
	if (swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > swapChainSupportDetails.capabilities.maxImageCount) {
		imageCount = swapChainSupportDetails.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface->Value();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	QueueFamilyIndices indices = physicalDevice->GetQueueFamilyIndices();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.computeFamily.value(), indices.transferFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 4;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VK_CHECK_MESSAGE(vkCreateSwapchainKHR(device->Value(), &createInfo, nullptr, &swapChain), "Failed to create swap chain!");

	vkGetSwapchainImagesKHR(device->Value(), swapChain, &imageCount, nullptr);

	swapChainImages.resize(imageCount);
	swapChainImageViews.resize(imageCount);

	vkGetSwapchainImagesKHR(device->Value(), swapChain, &imageCount, swapChainImages.data());

	for (uint32_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = swapChainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = surfaceFormat.format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VK_CHECK_MESSAGE(vkCreateImageView(device->Value(), &viewInfo, nullptr, &swapChainImageViews[i]), "Failed to create swap chain image view!");
	}

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Vk::SwapChain::Destroy()
{
	//Legacy Vulkan 1.0
	//Destroy depthbuffer
	//Destroy framebuffers

	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device->Value(), imageView, nullptr);
	}

	vkDestroySwapchainKHR(device->Value(), swapChain, nullptr);
}

Vk::SwapChainSupportDetails Vk::SwapChain::QuerySwapChainSupportDetails()
{
	SwapChainSupportDetails details;

	//Surface Capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->Value(), surface->Value(), &details.capabilities);

	//Surface Formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->Value(), surface->Value(), &formatCount, nullptr);

	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->Value(), surface->Value(), &formatCount, details.formats.data());
	}

	//Surface Present Modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->Value(), surface->Value(), &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->Value(), surface->Value(), &presentModeCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR Vk::SwapChain::ChooseSwapSurfaceFormat()
{
	for (const auto& availableFormat : swapChainSupportDetails.formats)
	{
		if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return swapChainSupportDetails.formats[0];
}

VkPresentModeKHR Vk::SwapChain::ChooseSwapPresentMode()
{
	for (const auto& availablePresentMode : swapChainSupportDetails.presentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Vk::SwapChain::ChooseSwapExtent()
{
	if (swapChainSupportDetails.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return swapChainSupportDetails.capabilities.currentExtent;
	}
	else {
		auto [width, height] = windowExtentFunction();

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, swapChainSupportDetails.capabilities.minImageExtent.width, swapChainSupportDetails.capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, swapChainSupportDetails.capabilities.minImageExtent.height, swapChainSupportDetails.capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
