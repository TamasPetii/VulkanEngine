#pragma once
#include "System.h"

class ENGINE_API ShapeSystem : public System
{
public:
	virtual void OnStart(std::shared_ptr<Registry> registry) override;
	virtual void OnUpdate(std::shared_ptr<Registry> registry, float deltaTime) override;
	virtual void OnFinish(std::shared_ptr<Registry> registry) override;
	virtual void OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ComponentBufferManager> componentBufferManager, uint32_t frameIndex) override;
};

