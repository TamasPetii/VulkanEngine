#pragma once
#include "../EngineApi.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>

struct ENGINE_API Vertex
{
	Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& color, const glm::vec2& uv);

	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec3 color;
	float padding;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
};
