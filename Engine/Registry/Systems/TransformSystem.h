#pragma once
#include "System.h"
#include "Engine/Registry/Components/TransformComponent.h"
#include "Engine/Render/RenderContext.h"

class TransformSystem : public System
{
public:
	virtual void OnStart(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry) override;
	virtual void OnUpdate(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry, std::shared_ptr<ComponetBufferManager> componentBufferManager) override;
	virtual void OnFinish(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry) override;
private:
	virtual void RegisterComponentBitset(std::shared_ptr<Registry<DEFAULT_MAX_COMPONENTS>> registry) override;
};

