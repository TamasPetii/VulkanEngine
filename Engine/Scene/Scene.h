#pragma once
#include "Engine/Registry/Registry.h"
#include "Engine/Systems/Systems.h"
#include "Engine/Components/Components.h"
#include "Engine/Managers/ResourceManager.h"
#include "Engine/Timer/Timer.h"

class ENGINE_API Scene
{
public:
	Scene(std::shared_ptr<ResourceManager> resourceManager);
	~Scene();
	void Update(std::shared_ptr<Timer> frameTimer, uint32_t frameIndex);
	void UpdateGPU(uint32_t frameIndex);
	std::shared_ptr<Registry> GetRegistry() { return registry; }
private:
	void Initialize();
	void Cleanup();
	void UpdateSystems(float deltaTime);
	void FinishSystems();
	void UpdateSystemsGPU(uint32_t frameIndex);
private:
	void InitializeSystems();
	void InitializeRegistry();
private:
	std::shared_ptr<ResourceManager> resourceManager;

	Entity sceneCameraEntity = NULL_ENTITY;
	std::shared_ptr<Registry> registry;
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
};

