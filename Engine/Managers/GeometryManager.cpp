#include "GeometryManager.h"

GeometryManager* GeometryManager::manager = nullptr;

GeometryManager::GeometryManager()
{
	Initialize();
}

GeometryManager* GeometryManager::GetManager()
{
	if (manager == nullptr)
		manager = new GeometryManager();

	return manager;
}

void GeometryManager::Destroy()
{
	if (manager != nullptr)
	{
		delete manager;
		manager = nullptr;
	}
}

GeometryManager::~GeometryManager()
{
	Cleanup();
}

void GeometryManager::Initialize()
{
	shapes["sphere"] = std::make_shared<Sphere>();
	shapes["quad"] = std::make_shared<Quad>();
	shapes["capsule"] = std::make_shared<Capsule>();
	shapes["cube"] = std::make_shared<Cube>();
	shapes["cylinder"] = std::make_shared<Cylinder>();
	shapes["cone"] = std::make_shared<Cone>();
	shapes["pyramid"] = std::make_shared<Pyramid>();
	shapes["torus"] = std::make_shared<Torus>();

	shapes["proxySphere"] = std::make_shared<Sphere>(16);
	shapes["proxyCone"] = std::make_shared<Cone>(16);
}

void GeometryManager::Cleanup()
{
	shapes.clear();
}
