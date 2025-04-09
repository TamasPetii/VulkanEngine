#pragma once
#include <memory>
#include <algorithm>
#include <execution>

#include "Engine/EngineApi.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ComponentBufferManager.h"

class ENGINE_API System
{
public:
	virtual ~System() = default;
	virtual void OnStart(std::shared_ptr<Registry> registry) = 0;
	virtual void OnUpdate(std::shared_ptr<Registry> registry) = 0;
	virtual void OnFinish(std::shared_ptr<Registry> registry) = 0;
	virtual void OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ComponentBufferManager> componentBufferManager, uint32_t frameIndex) = 0;
};