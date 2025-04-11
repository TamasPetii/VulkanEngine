#pragma once
#include <glm/glm.hpp>
#include "Engine/Render/Vertex.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"

class Renderable
{
public:
	virtual void UploadToGpu();
	const auto& GetIndexCount() const { return indexCount; }
	const auto& GetVertexCount() const { return vertexCount; }
	const auto& GetSurfacePoints() const { return surfacePoints; }
	const auto& GetVertexBuffer()const { return vertexBuffer; }
	const auto& GetIndexBuffer() const { return indexBuffer; }
protected:
	uint32_t indexCount = 0;
	uint32_t vertexCount = 0;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<glm::vec3> surfacePoints;
	std::shared_ptr<Vk::Buffer> indexBuffer;
	std::shared_ptr<Vk::Buffer> vertexBuffer;
	std::shared_ptr<Vk::Buffer> instanceBuffer;
};

