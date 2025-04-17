#pragma once
#include "Component.h"
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

	/*
	//This is parent-child transformed data
	glm::vec3 fullTranslation;
	glm::mat4 fullTranslationMatrix;
	glm::vec3 fullRotation;
	glm::mat4 fullRotationMatrix;
	glm::vec3 fullScale;
	glm::mat4 fullScaleMatrix;
	*/
};

struct ENGINE_API TransformComponentGPU
{
	TransformComponentGPU(const TransformComponent& component);

	glm::mat4 transform;
	glm::mat4 transformIT;
};