#pragma once
#include "System.h"
#include "Engine/Renderable/BoundingVolume.h"
#include "Engine/Components/TransformComponent.h"
#include "Engine/Components/DefaultColliderComponent.h"

class ENGINE_API DefaultColliderSystem : public System
{
public:
	virtual void OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, float deltaTime) override;
	virtual void OnFinish(std::shared_ptr<Registry> registry) override;
	virtual void OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex) override;
private:
	void UpdateDefaultComponentsWithShapes(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager);
	void UpdateDefaultComponentsWithModels(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager);
	inline void CalculateDefaultColliderParameters(DefaultColliderComponent* defaultColliderComponent, TransformComponent* transformComponent, std::shared_ptr<BoundingVolume> boundingVolume);
};

