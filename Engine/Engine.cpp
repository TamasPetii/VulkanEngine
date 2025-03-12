#include "Engine.h"

Engine::Engine()
{
	Init();
}

Engine::~Engine()
{
	Clean();
}

void Engine::Init()
{
	frameTimer = std::make_unique<FrameTimer>();

	if (Vk::ValidationLayer::ValidationLayerEnabled())
		SetRequiredExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	instance = std::make_unique<Vk::Instance>(requiredExtensions);
	debugMessenger = std::make_unique<Vk::DebugMessenger>(instance.get());
}

void Engine::Clean()
{
}

void Engine::SetRequiredExtension(const char* extensionName)
{
	requiredExtensions.push_back(extensionName);
}

void Engine::Update()
{
	frameTimer->Update();
}

void Engine::Render()
{
}

