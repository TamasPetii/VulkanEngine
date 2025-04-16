#pragma once
#include "ColliderGJK.h"

struct ENGINE_API SphereColliderGJK : public ColliderGJK
{
	glm::vec3 FindFurthestPoint(const glm::vec3& direction) const override;
	float radius;
	glm::vec3 origin;
};

