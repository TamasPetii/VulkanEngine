#pragma once
#include "ImageManager.h"
#include "VulkanManager.h"
#include "GeometryManager.h"
#include "ComponentBufferManager.h"
#include "ModelManager.h"
#include "BenchmarkManager.h"

class ENGINE_API ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	void Initialize();
	void Cleanup();

	auto GetModelManager() { return modelManager; }
	auto GetImageManager() { return imageManager; }
	auto GetVulkanManager() { return vulkanManager; }
	auto GetGeometryManager() { return geometryManager; }
	auto GetComponentBufferManager() { return componentBufferManager; }
private:
	std::shared_ptr<ModelManager> modelManager;
	std::shared_ptr<ImageManager> imageManager;
	std::shared_ptr<VulkanManager> vulkanManager;
	std::shared_ptr<GeometryManager> geometryManager;
	std::shared_ptr<ComponentBufferManager> componentBufferManager;
	std::shared_ptr<BenchmarkManager> benchmarkManager;
};

