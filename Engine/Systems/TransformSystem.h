#pragma once
#include "System.h"
#include "Engine/Components/TransformComponent.h"


class TransformSystem : public System
{
public:
	virtual void OnStart(std::shared_ptr<Registry> registry) override;
	virtual void OnUpdate(std::shared_ptr<Registry> registry) override;
	virtual void OnFinish(std::shared_ptr<Registry> registry) override;
	virtual void OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ComponentBufferManager> componentBufferManager, uint32_t frameIndex) override;
};

