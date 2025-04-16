#include "SphereColliderGJK.h"

glm::vec3 SphereColliderGJK::FindFurthestPoint(const glm::vec3& direction) const
{
	return origin + glm::normalize(direction) * radius;
}
