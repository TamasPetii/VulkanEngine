#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>
#include <array>
#include <vector>

class ENGINE_API BoundingVolume
{
public:
	const auto& GetSurfacePoints() const { return surfacePoints; }
protected:
	virtual void PopulateSurfacePoints() = 0;
	void GenerateBoundingVolume();
protected:
	glm::vec3 aabbMax;
	glm::vec3 aabbMin;
	glm::vec3 aabbOrigin;
	glm::vec3 aabbExtents;
	std::array<glm::vec3, 8> obb;
	std::vector<glm::vec3> surfacePoints;
};
