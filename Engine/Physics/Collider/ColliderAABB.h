#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>

struct ENGINE_API ColliderAABB
{
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 origin;
	glm::vec3 extents;
};

