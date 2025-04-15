#pragma once
#include "Component.h"
#include "ComponentGPU.h"
#include <glm/glm.hpp>

struct ENGINE_API CameraComponent : public Component
{
	CameraComponent();

	float yaw;
	float pitch;

	float nearPlane;
	float farPlane;

	float fov;
	float width;
	float height;
	float speed;
	float sensitivity;
	float distance;

	glm::vec3 up;
	glm::vec3 target;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 right;

	glm::mat4 view;
	glm::mat4 viewInv;
	glm::mat4 proj;
	glm::mat4 projInv;
	glm::mat4 viewProj;
	glm::mat4 viewProjInv;
};

struct ENGINE_API CameraComponentGPU
{
public:
	CameraComponentGPU(const CameraComponent& component);

	glm::mat4 view;
	glm::mat4 viewInv;
	glm::mat4 proj;
	glm::mat4 projInv;
	glm::mat4 viewProj;
	glm::mat4 viewProjInv;
	glm::vec4 eye;
};
