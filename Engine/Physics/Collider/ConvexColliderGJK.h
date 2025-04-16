#pragma once
#include "ColliderGJK.h"
#include <vector>

struct ENGINE_API ConvexColliderGJK : public ColliderGJK
{
	glm::vec3 FindFurthestPoint(const glm::vec3& direction) const override;
	std::vector<glm::vec3> surfacePoints;
};

using BoxColliderGJK = ConvexColliderGJK;
