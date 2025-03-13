#pragma once
#include "../EngineApi.h"
#include "Instance.h"
#include "DebugMessenger.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"
#include "SwapChain.h"
#include "ValidationLayer.h"
#include <vector>

class Engine;

namespace Vk
{
	class ENGINE_API VulkanContext
	{
	public:
		~VulkanContext();
		static VulkanContext* GetContext();
		static void DestroyContext();
		void SetRequiredInstanceExtension(const char* extensionName);
		void SetRequiredInstanceExtensions(std::span<const char*> extensionNames);
		void SetRequiredDeviceExtension(const char* extensionName);
		void SetRequiredDeviceExtensions(std::span<const char*> extensionNames);
		void SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function);
		void SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function);
		const Vk::Instance* const GetInstance();
		const Vk::PhysicalDevice* const GetPhysicalDevice();
		const Vk::Device* const GetDevice();
		const Vk::Surface* const GetSurface();
		Vk::SwapChain* GetSwapChain();
	private:
		VulkanContext() = default;
		void Init();
		void Destory();
		static VulkanContext* context;
		static bool initialized;
	private:
		std::unique_ptr<Vk::Instance> instance;
		std::unique_ptr<Vk::DebugMessenger> debugMessenger;
		std::unique_ptr<Vk::Surface> surface;
		std::unique_ptr<Vk::PhysicalDevice> physicalDevice;
		std::unique_ptr<Vk::Device> device;
		std::unique_ptr<Vk::SwapChain> swapChain;

		std::vector<const char*> requiredDeviceExtensions;
		std::vector<const char*> requiredInstanceExtensions;
		std::function<std::pair<int, int>()> windowExtentFunction;
		std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)> surfaceCreationFunction;

		friend class Engine;
	};
}


