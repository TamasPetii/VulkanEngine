#pragma once
#include "System.h"

class SpotLightSystem : public System
{
    virtual void OnUpdate(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex, float deltaTime) override;
    virtual void OnFinish(std::shared_ptr<Registry> registry) override;
    virtual void OnUploadToGpu(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex) override;
};

