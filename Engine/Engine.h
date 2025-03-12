#pragma once
#include "EngineApi.h"
#include "../Engine/Logger/Checker.h"
#include "Timer/FrameTimer.h"

#include "Vulkan/Instance.h"
#include "Vulkan/DebugMessenger.h"
#include "Vulkan/ValidationLayer.h"

class ENGINE_API Engine
{
public:
	Engine();
	~Engine();
	void Update();
	void Render();
	void SetRequiredExtension(const char* extensionName);
private:
	void Init();
	void Clean();
private:
	std::unique_ptr<FrameTimer> frameTimer;
	std::vector<const char*> requiredExtensions;
	std::unique_ptr<Vk::Instance> instance;
	std::unique_ptr<Vk::DebugMessenger> debugMessenger;
};

