#pragma once
#include "System.h"
#include "Engine/Renderable/BoundingVolume.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/DefaultColliderComponent.h"

class ENGINE_API DefaultColliderSystem : public System
{
public:
	virtual void OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, float deltaTime) override;
	virtual void OnFinish(std::shared_ptr<Registry> registry) override;
	virtual void OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex) override;
};

