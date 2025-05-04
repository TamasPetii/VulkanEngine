#pragma once
#include "../EngineApi.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>

struct ENGINE_API Vertex
{
	Vertex() = default;
	Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv, const glm::vec3& tan = glm::vec3(0, 0, 0), const glm::vec3& bitan = glm::vec3(0, 0, 0), uint32_t matIndex = UINT32_MAX, uint32_t nodeIdx = UINT32_MAX)
		: position(pos), uv_x(uv.x), normal(norm), uv_y(uv.y), tangent(tan), matIndex(matIndex), bitangent(bitan), nodeIndex(nodeIdx) {
	}

	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec3 tangent;
	uint32_t matIndex;
	glm::vec3 bitangent;
	uint32_t nodeIndex;
};
