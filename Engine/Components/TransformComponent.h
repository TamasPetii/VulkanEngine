#pragma once
#include "Component.h"
#include "ComponentGPU.h"
#include <glm/glm.hpp>

struct ENGINE_API TransformComponent : public Component
{
	TransformComponent();

	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 transform;
	glm::mat4 transformIT;
};

struct ENGINE_API TransformComponentGPU
{
	TransformComponentGPU(const TransformComponent& component);

	glm::mat4 transform;
	glm::mat4 transformIT;
};