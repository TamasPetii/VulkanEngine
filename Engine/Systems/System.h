#pragma once
#include <memory>
#include <algorithm>
#include <execution>

#include "Engine/EngineApi.h"
#include "Engine/Registry/Registry.h"
#include "Engine/Managers/ComponetBufferManager.h"

class ENGINE_API System
{
public:
	virtual ~System() = default;
	virtual void OnStart(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry) = 0;
	virtual void OnUpdate(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry, std::shared_ptr<ComponetBufferManager> componentBufferManager) = 0;
	virtual void OnFinish(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry) = 0;
protected:
	virtual void RegisterComponentBitset(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry) = 0;
};