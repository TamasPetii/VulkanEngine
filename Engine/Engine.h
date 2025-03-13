#pragma once
#include "EngineApi.h"
#include "../Engine/Logger/Checker.h"
#include "Timer/FrameTimer.h"

#include "Vulkan/Instance.h"
#include "Vulkan/DebugMessenger.h"
#include "Vulkan/ValidationLayer.h"
#include "Vulkan/Surface.h"
#include "Vulkan/PhysicalDevice.h"
#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"

class ENGINE_API Engine
{
public:
	Engine();
	~Engine();
	void Init();
	void Update();
	void Render();
public:
	void SetRequiredInstanceExtension(const char* extensionName);
	void SetRequiredInstanceExtensions(std::span<const char*> extensionNames);
	void SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function);
	void SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function);
private:
	void SetRequiredDeviceExtension(const char* extensionName);
	void SetRequiredDeviceExtensions(std::span<const char*> extensionNames);
	void Clean();
private:
	std::unique_ptr<Vk::Instance> instance;
	std::unique_ptr<Vk::DebugMessenger> debugMessenger;
	std::unique_ptr<Vk::Surface> surface;
	std::unique_ptr<Vk::PhysicalDevice> physicalDevice;
	std::unique_ptr<Vk::Device> device;
	std::unique_ptr<Vk::SwapChain> swapChain;

	std::unique_ptr<FrameTimer> frameTimer;
	std::vector<const char*> requiredDeviceExtensions;
	std::vector<const char*> requiredInstanceExtensions;
	std::function<std::pair<int, int>()> windowExtentFunction;
	std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)> surfaceCreationFunction;
};

