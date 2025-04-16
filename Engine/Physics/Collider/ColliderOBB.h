#pragma once
#include "ColliderGJK.h"
#include <glm/glm.hpp>
#include <array>

struct ENGINE_API ColliderOBB : public ColliderGJK
{
	virtual glm::vec3 FindFurthestPoint(const glm::vec3& direction) const override;
	std::array<glm::vec3, 8> obbPositions;
};

