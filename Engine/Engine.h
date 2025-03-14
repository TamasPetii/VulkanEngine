#pragma once
#include "EngineApi.h"
#include "../Engine/Logger/Checker.h"
#include "Timer/FrameTimer.h"

#include "Vulkan/VulkanContext.h"
#include "Vulkan/ShaderModule.h"

#include "Registry/Registry.h"

class ENGINE_API Engine
{
public:
	Engine();
	~Engine();
	void Init();
	void Update();
	void Render();
	void WindowResizeEvent();
public:
	void SetRequiredWindowExtensions(std::span<const char*> extensionNames);
	void SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function);
	void SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function);
private:
	void Clean();
private:
	bool isWindowResized;
	std::shared_ptr<Registry> registry;
	std::unique_ptr<FrameTimer> frameTimer;
};