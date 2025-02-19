#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <dwmapi.h>
#include <cstdlib>
#include <vector>
#include <optional>

#include "Logger.h"

#define VK_CHECK(functionCall, errorMessage) if (functionCall != VK_SUCCESS) { Logger::Log("VulkanApp", errorMessage); throw std::runtime_error("Vulkan Error"); }

struct QueueFamilyIndices 
{
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() { return graphicsFamily.has_value(); }
};

class VulkanApp
{
public:
	void Run();
	void CreateInstance();
private:
	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void Cleanup();
	bool CheckValidationLayerSupport();
	void SetupDebugMessenger();
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	std::vector<const char*> GetRequiredExtensions();
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	void CreateLogicalDevice();
private:
	GLFWwindow* window = VK_NULL_HANDLE;
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;

	static bool enableValidationLayers;
	static std::vector<const char*> validationLayers;
};