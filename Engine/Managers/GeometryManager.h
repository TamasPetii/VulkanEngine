#pragma once
#include "Engine/Geometry/Geometries.h"

#include <memory>
#include <string>
#include <unordered_map>

class ENGINE_API GeometryManager
{
public:
	GeometryManager();
	~GeometryManager();
	std::shared_ptr<Shape> GetShape(const std::string& name) const;
private:
	void Initialize();
	void Cleanup();
private:
	std::unordered_map<std::string, std::shared_ptr<Shape>> shapes;
};

