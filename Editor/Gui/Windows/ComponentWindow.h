#pragma once
#include <memory>

class Registry;
class ResourceManager;

class ComponentWindow
{
public:
	static void Render(std::shared_ptr<Registry> registry, std::shared_ptr<ResourceManager> resourceManager, uint32_t frameIndex);
};

