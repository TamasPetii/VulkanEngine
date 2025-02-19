#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <glfw/glfw3native.h>

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <dwmapi.h>
#include <cstdlib>
#include <vector>

#include "Logger.h"

#define VK_CHECK(functionCall, errorMessage) if (functionCall != VK_SUCCESS) { Logger::Log("VulkanApp", errorMessage); throw std::runtime_error("Vulkan Error"); }

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
private:
	GLFWwindow* window = nullptr;
	VkInstance instance = nullptr;

	VkDebugUtilsMessengerEXT debugMessenger = nullptr;
	static bool enableValidationLayers;
	static std::vector<const char*> validationLayers;
};