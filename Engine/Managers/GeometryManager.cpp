#include "GeometryManager.h"

GeometryManager::GeometryManager()
{
	Initialize();
}

GeometryManager::~GeometryManager()
{
	Cleanup();
}

void GeometryManager::Initialize()
{
	shapes["Sphere"] = std::make_shared<Sphere>();
	shapes["Quad"] = std::make_shared<Quad>();
	shapes["Capsule"] = std::make_shared<Capsule>();
	shapes["Cube"] = std::make_shared<Cube>();
	shapes["Cylinder"] = std::make_shared<Cylinder>();
	shapes["Cone"] = std::make_shared<Cone>();
	shapes["Pyramid"] = std::make_shared<Pyramid>();
	shapes["Torus"] = std::make_shared<Torus>();
	shapes["ProxySphere"] = std::make_shared<Sphere>(16);
	shapes["ProxyCone"] = std::make_shared<Cone>(16);
}

void GeometryManager::Cleanup()
{
	shapes.clear();
}

std::shared_ptr<Shape> GeometryManager::GetShape(const std::string& name) const
{
	if (shapes.find(name) == shapes.end())
		return nullptr;

	return shapes.at(name);
}
