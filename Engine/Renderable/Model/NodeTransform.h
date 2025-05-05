#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>

struct ENGINE_API NodeTransform
{
	glm::mat4 transform = glm::mat4(1);
	glm::mat4 transformIT = glm::mat4(1);
};
