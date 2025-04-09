#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
	Initialize();
}

ResourceManager::~ResourceManager()
{
	Cleanup();
}

void ResourceManager::Initialize()
{
	vulkanManager = std::make_shared<VulkanManager>();
	geometryManager = std::make_shared<GeometryManager>();
	componentBufferManager = std::make_shared<ComponentBufferManager>();
}

void ResourceManager::Cleanup()
{
	geometryManager.reset();
	vulkanManager.reset();
	componentBufferManager.reset();
}
