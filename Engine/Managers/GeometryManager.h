#pragma once
#include "Engine/Geometry/Geometries.h"

#include <memory>
#include <string>
#include <unordered_map>

class GeometryManager
{
public:
	static GeometryManager* GetManager();
	static void Destroy();

	~GeometryManager();
	const auto& GetShape(const std::string& shapeName) const { return shapes.at(shapeName); }
private:
	static GeometryManager* manager;
	GeometryManager();
	void Initialize();
	void Cleanup();
private:
	std::unordered_map<std::string, std::shared_ptr<Shape>> shapes;
};

