#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Engine/Render/Vertex.h"
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"

class Shape
{
public:
	const auto& GetIndexCount() const { return indexCount; }
	const auto& GetVertexCount() const { return vertexCount; }
	const auto& GetSurfacePoints() const { return surfacePoints; }
	const auto& GetVertexBuffer()const { return vertexBuffer; }
	const auto& GetIndexBuffer() const { return indexBuffer; }
protected:
	void Initialize();
	void UploadToGpu();
	virtual void GenerateSurfacePoints() = 0;
	virtual void GenerateVertices() = 0;
	virtual void GenerateIndices() = 0;
	void GenerateTangents();
protected:
	uint32_t indexCount;
	uint32_t vertexCount;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<glm::vec3> surfacePoints;
	std::shared_ptr<Vk::Buffer> indexBuffer;
	std::shared_ptr<Vk::Buffer> vertexBuffer;
};

