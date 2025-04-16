#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>

struct ENGINE_API ColliderGJK
{
	virtual glm::vec3 FindFurthestPoint(const glm::vec3& direction) const = 0;
};

