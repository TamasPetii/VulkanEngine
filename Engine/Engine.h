#pragma once
#include "EngineApi.h"
#include "../Engine/Logger/Checker.h"
#include "Registry/Registry.h"
#include "Render/Renderer.h"
#include <span>
#include <functional>

#include "Engine/Components/Components.h"
#include "Engine/Systems/Systems.h"
#include "Engine/Managers/InputManager.h"
#include "Engine/Managers/ResourceManager.h"

#include "Timer/Timer.h"

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
	void SystemFinish();
	void SystemUpdate(float deltaTime);
	void SystemUpdateGPU();
private:
	void InitSystems();
	void InitRegistry();
	void InitComponentBufferManager();
	void CheckForComponentBufferResize();
private:
	uint32_t framesInFlightIndex = 0;

	std::shared_ptr<Timer> frameTimer;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Registry> registry;
	std::shared_ptr<ResourceManager> resourceManager;

	std::unordered_map<std::type_index, double> systemTimes;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
};