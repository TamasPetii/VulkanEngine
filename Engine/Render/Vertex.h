#pragma once
#include "../EngineApi.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>

struct ENGINE_API Vertex
{
	Vertex() = default;
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv);
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec2& uv);
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec2& uv, uint32_t index);
	static VkVertexInputBindingDescription GetBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec3 tangent;
	uint32_t index;
};
