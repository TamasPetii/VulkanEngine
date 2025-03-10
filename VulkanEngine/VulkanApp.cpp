#include "VulkanApp.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <dwmapi.h>

void VulkanApp::Run()
{
	Init();
	MainLoop();
	Cleanup();
}

void VulkanApp::Init()
{
	InitWindow();
	InitVulkan();
	InitSwapChain();
	InitCommands();
	InitSyncStructures();
}

void VulkanApp::MainLoop()
{
	auto startTime = std::chrono::high_resolution_clock::now();
	float prevTime = 0.0f;
	
	float fpsTimer = 0.0f;
	int frameCount = 0;

	while (!glfwWindowShouldClose(window))
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		deltaTime = time - prevTime;
		prevTime = time;

		fpsTimer += deltaTime;
		frameCount++;

		if (fpsTimer > 1.f)
		{
			float fps = frameCount / fpsTimer;
			frameCount = 0;
			fpsTimer = 0;

			std::string title = "Vulkan Engine | " + std::to_string(static_cast<int>(fps)) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		glfwPollEvents();
		Draw();

		frame++;
	}
}

void VulkanApp::Cleanup()
{
	vkDeviceWaitIdle(logicalDevice);

	for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		vkDestroyCommandPool(logicalDevice, frameData[i].commandPool, nullptr);
		vkDestroyFence(logicalDevice, frameData[i].renderFence, nullptr);
		vkDestroySemaphore(logicalDevice, frameData[i].renderSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, frameData[i].swapchainSemaphore, nullptr);
	}

	DestroySwapchain();
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	vkb::destroy_debug_utils_messenger(instance, debugMessenger);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
}

void VulkanApp::Draw()
{
	VK_CHECK(vkWaitForFences(logicalDevice, 1, &GetCurrentFrameData().renderFence, true, UINT64_MAX), "Wait for fences error");
	VK_CHECK(vkResetFences(logicalDevice, 1, &GetCurrentFrameData().renderFence), "Reset fences error");

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(logicalDevice, swapchain, UINT64_MAX, GetCurrentFrameData().swapchainSemaphore, nullptr, &swapchainImageIndex), "Couldn't acquire swapchain image!");

	VkCommandBuffer commandBuffer = GetCurrentFrameData().commandBuffer;
	VK_CHECK(vkResetCommandBuffer(commandBuffer, 0), "Couldn't reset command buffer");

	VkCommandBufferBeginInfo commandBufferBeginInfo = Vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "Couldn't begin command buffer!");

	Vkimages::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	VkClearColorValue clearValue;
	float flash = std::abs(std::sin(frame / 120.f));
	clearValue = { { 0.0f, 0.0f, flash, 1.0f } };
	VkImageSubresourceRange clearRange = Vkinit::ImageSubResourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

	vkCmdClearColorImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	Vkimages::TransitionImage(commandBuffer, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	VK_CHECK(vkEndCommandBuffer(commandBuffer), "Error occured while ending command buffer!");

	VkCommandBufferSubmitInfo commandSubmitInfo = Vkinit::CommandBufferSubmitInfo(commandBuffer);
	VkSemaphoreSubmitInfo waitSubmitInfo = Vkinit::SemaphoreSubmintInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrameData().swapchainSemaphore);
	VkSemaphoreSubmitInfo signalSubmitInfo = Vkinit::SemaphoreSubmintInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrameData().renderSemaphore);
	VkSubmitInfo2 submitInfo = Vkinit::SubmitInfo(&commandSubmitInfo, &signalSubmitInfo, &waitSubmitInfo);

	VK_CHECK(vkQueueSubmit2(graphicsQueue, 1, &submitInfo, GetCurrentFrameData().renderFence), "Couldn't submit command to graphics queue!");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain;
	presentInfo.pImageIndices = &swapchainImageIndex;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &GetCurrentFrameData().renderSemaphore;
	presentInfo.pNext = nullptr;

	VK_CHECK(vkQueuePresentKHR(graphicsQueue, &presentInfo), "Couldn't start presentation operations!");

}

void VulkanApp::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);

	HWND hwnd = glfwGetWin32Window(window);
	COLORREF color = RGB(0, 0, 0);
	const DWORD DWMWA_CAPTION_COLOR = 35;
	HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));

	int width, height, channels;
	unsigned char* pixels = stbi_load("DendriteLogo.png", &width, &height, &channels, 0);

	if (pixels)
	{
		GLFWimage images[1];
		images[0].width = width;
		images[0].height = height;
		images[0].pixels = pixels;
		glfwSetWindowIcon(window, 1, images);
	}

	stbi_image_free(pixels);
}

void VulkanApp::InitVulkan()
{
	vkb::InstanceBuilder builder;

	auto buildedResource = builder.set_app_name("Vulkan Engine")
								  .request_validation_layers(true)
								  .use_default_debug_messenger()
								  .require_api_version(1, 3, 0)
								  .build();

	vkb::Instance vkbInstance = buildedResource.value();
	instance = vkbInstance;
	debugMessenger = vkbInstance.debug_messenger;

	VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface), "Failed to create window surface");

	VkPhysicalDeviceVulkan13Features features13{};
	features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features13.dynamicRendering = true;
	features13.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{};
	features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

	vkb::PhysicalDeviceSelector selector{ vkbInstance };

	vkb::PhysicalDevice vkbPhysicalDevice = selector.set_minimum_version(1, 3)
													.set_required_features_12(features12)
													.set_required_features_13(features13)
													.set_surface(surface)
													.select()
													.value();

	vkb::DeviceBuilder deviceBuilder{ vkbPhysicalDevice };
	vkb::Device vkbLogicalDevice = deviceBuilder.build().value();

	physicalDevice = vkbPhysicalDevice.physical_device;
	logicalDevice = vkbLogicalDevice.device;

	graphicsQueue = vkbLogicalDevice.get_queue(vkb::QueueType::graphics).value();
	graphicsQueueIndex = vkbLogicalDevice.get_queue_index(vkb::QueueType::graphics).value();
}

void VulkanApp::InitSwapChain()
{
	CreateSwapchain();
}

void VulkanApp::InitCommands()
{
	
	VkCommandPoolCreateInfo commandPoolInfo = Vkinit::CommandPoolCreateInfo(graphicsQueueIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		VK_CHECK(vkCreateCommandPool(logicalDevice, &commandPoolInfo, nullptr, &frameData[i].commandPool), "Couldn't create command pool!");

		VkCommandBufferAllocateInfo commandBufferInfo = Vkinit::CommandBufferAllocateInfo(frameData[i].commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(logicalDevice, &commandBufferInfo, &frameData[i].commandBuffer), "Couldn't allocate command buffer");
	}
}

void VulkanApp::InitSyncStructures()
{
	VkFenceCreateInfo fenceInfo = Vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreInfo = Vkinit::SemaphoreCreateInfo();

	for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
	{
		VK_CHECK(vkCreateFence(logicalDevice, &fenceInfo, nullptr, &frameData[i].renderFence), "Couldn't create render fence");
		VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &frameData[i].renderSemaphore), "Couldn't create render semaphore");
		VK_CHECK(vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &frameData[i].swapchainSemaphore), "Couldn't create swapchain semaphore");
	}
}

void VulkanApp::CreateSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{physicalDevice, logicalDevice, surface};
	swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	VkSurfaceFormatKHR surfaceFormat;
	surfaceFormat.format = swapchainImageFormat;
	surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	vkb::Swapchain vkbSwapchain = swapchainBuilder.set_desired_format(surfaceFormat)
												  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
												  .set_desired_extent(WIDTH, HEIGHT)
												  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
												  .build()
												  .value();
	swapchainExtent = vkbSwapchain.extent;
	swapchain = vkbSwapchain.swapchain;
	swapchainImages = vkbSwapchain.get_images().value();
	swapchainImageViews = vkbSwapchain.get_image_views().value();
}

void VulkanApp::DestroySwapchain()
{
	vkDestroySwapchainKHR(logicalDevice, swapchain, nullptr);

	for (int i = 0; i < swapchainImageViews.size(); i++) {

		vkDestroyImageView(logicalDevice, swapchainImageViews[i], nullptr);
	}
}
