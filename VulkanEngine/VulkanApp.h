#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#include <VkBootstrap.h>

#include "Vk/vk_types.h"
#include "Vk/vk_initializers.h"
#include "Vk/vk_images.h"
#include "Vk/vk_descriptors.h"
#include "Vk/vk_pipelines.h"

#include <chrono>
#include <thread>

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush()
	{
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)();
		}

		deletors.clear();
	}
};

struct FrameData
{
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkSemaphore swapchainSemaphore;
	VkSemaphore renderSemaphore;
	VkFence renderFence;
};

constexpr uint32_t FRAME_OVERLAP = 2;

class VulkanApp
{
public:
	void Run();
	void Init();
	void MainLoop();
	void Cleanup();
private:
	void Draw();
	void DrawBackground();
	void InitWindow();
	void InitVulkan();
	void InitSwapChain();
	void InitCommands();
	void InitSyncStructures();
	void InitDescriptors();
	void InitPipelines();
	void InitBackgroundPipelines();
	void CreateSwapchain();
	void DestroySwapchain();
private:
	GLFWwindow* window = nullptr;
	VkInstance instance = nullptr;
	VkDebugUtilsMessengerEXT debugMessenger = nullptr;
	VkPhysicalDevice physicalDevice = nullptr;
	VkDevice logicalDevice = nullptr;
	VkSurfaceKHR surface = nullptr;
	VkSwapchainKHR swapchain = nullptr;
	VkFormat swapchainImageFormat;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	VkExtent2D swapchainExtent;
	VkQueue graphicsQueue;
	uint32_t graphicsQueueIndex;
	FrameData frameData[FRAME_OVERLAP];
	FrameData& GetCurrentFrameData() { return frameData[frame % FRAME_OVERLAP]; }

	VmaAllocator vmaAllocator;
	AllocatedImage drawImage;
	VkExtent2D drawExtent;

	DescriptorAllocator globalDescriptorAllocator;
	VkDescriptorSet drawImageDescriptor;
	VkDescriptorSetLayout drawImageDescriptorLayout;

	VkPipeline gradientPipeline;
	VkPipelineLayout gradientPipelineLayout;
private:
	uint32_t frame = 0;
	uint32_t currentFrame = 0;
	float time = 0;
	float deltaTime = 0;
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
};