#pragma once
#include "Engine/EngineApi.h"
#include <glm/glm.hpp>
#include <array>

struct ENGINE_API FrustumFace
{
	FrustumFace() = default;
	FrustumFace(const glm::vec3& normal, float distance);
	FrustumFace(const glm::vec3& normal, const glm::vec3 point);

	glm::vec3 normal;
	float distance;
};

struct ENGINE_API FrustumCollider
{
	std::array<FrustumFace, 6> faces;
};

