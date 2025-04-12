#include "Vertex.h"


Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& uv) :
	position(position),
	normal(normal)
{
	uv_x = uv.x;
	uv_y = uv.y;
	index = 0;
}

Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec2& uv) :
	position(position),
	normal(normal),
	tangent(tangent)
{
	uv_x = uv.x;
	uv_y = uv.y;
	index = 0;
}

Vertex::Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec2& uv, uint32_t index) :
	position(position),
	uv_x(uv.x),
	normal(normal),
	uv_y(uv.y),
	tangent(tangent),
	index(index)
{
}

VkVertexInputBindingDescription Vertex::GetBindingDescription() {
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	/*
	{
		VkVertexInputAttributeDescription attributeDescription = {};
		attributeDescription.binding = 0;
		attributeDescription.location = 0;
		attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription.offset = offsetof(Vertex, position);
		attributeDescriptions.push_back(attributeDescription);
	}

	{
		VkVertexInputAttributeDescription attributeDescription = {};
		attributeDescription.binding = 0;
		attributeDescription.location = 1;
		attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescription.offset = offsetof(Vertex, normal);
		attributeDescriptions.push_back(attributeDescription);
	}

	{
		VkVertexInputAttributeDescription attributeDescription = {};
		attributeDescription.binding = 0;
		attributeDescription.location = 2;
		attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescription.offset = offsetof(Vertex, texCoord);
		attributeDescriptions.push_back(attributeDescription);
	}
	*/

	return attributeDescriptions;
}