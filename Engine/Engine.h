#pragma once
#include "EngineApi.h"
#include "../Engine/Logger/Checker.h"
#include "Timer/FrameTimer.h"
#include "Registry/Registry.h"
#include "Render/Renderer.h"
#include <span>
#include <functional>


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
	void SetGuiRenderFunction(const std::function<void(VkCommandBuffer commandBuffer)>& function);
private:
	void Clean();
private:
	bool isWindowResized;
	std::shared_ptr<Registry<32>> registry;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<FrameTimer> frameTimer;
};