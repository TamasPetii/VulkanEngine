#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>

struct ENGINE_API ColliderAABB
{
	glm::vec3 aabbMin;
	glm::vec3 aabbMax;
	glm::vec3 aabbOrigin;
	glm::vec3 aabbExtents;
};

