#pragma once
#include "EngineApi.h"
#include <span>
#include <functional>

#include "Engine/Timer/Timer.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Managers/InputManager.h"

class ENGINE_API Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void SimulateFrame();
	void WindowResizeEvent();
	void SetRequiredWindowExtensions(std::span<const char*> extensionNames);
	void SetSurfaceCreationFunction(const std::function<void(const Vk::Instance* const, VkSurfaceKHR* surface)>& function);
	void SetWindowExtentFunction(const std::function<std::pair<int, int>()>& function);
	void SetGuiRenderFunction(const std::function<void(VkCommandBuffer, std::shared_ptr<Registry>, std::shared_ptr<ResourceManager>, uint32_t)>& function);
private:
	void Cleanup();
	void Render();
	void Update();
	void UpdateGPU();
private:
	void InitComponentBufferManager();
	void CheckForComponentBufferResize();
private:
	uint32_t framesInFlightIndex = 0;
	std::shared_ptr<Scene> scene;
	std::shared_ptr<Timer> frameTimer;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<ResourceManager> resourceManager;
};