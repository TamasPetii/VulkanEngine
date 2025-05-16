#pragma once
#include "BaseComponents/Component.h"
#include <glm/glm.hpp>

struct ENGINE_API TransformComponent : public Component
{
	TransformComponent();

	//This is entity transform data
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