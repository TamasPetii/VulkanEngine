#include "Shape.h"

void Shape::Initialize()
{
	GenerateSurfacePoints();
	GenerateVertices();
	GenerateIndices();
	GenerateTangents();
	UploadToGpu();
}

void Shape::UploadToGpu()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	//Upload the vertices to the gpu
	{
		VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

		Vk::BufferConfig stagingConfig;
		stagingConfig.size = bufferSize;
		stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
		stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Vk::Buffer stagingBuffer{ stagingConfig };

		void* mappedBuffer = stagingBuffer.MapMemory();
		memcpy(mappedBuffer, vertices.data(), (size_t)bufferSize);
		stagingBuffer.UnmapMemory();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		vertexBuffer = std::make_shared<Vk::Buffer>(config);

		Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
			[&](VkCommandBuffer commandBuffer) -> void {
				Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), vertexBuffer->Value(), bufferSize);
			}
		);
	}

	//Upload the indices to the gpu
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

		Vk::BufferConfig stagingConfig;
		stagingConfig.size = bufferSize;
		stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
		stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Vk::Buffer stagingBuffer{ stagingConfig };

		void* mappedBuffer = stagingBuffer.MapMemory();
		memcpy(mappedBuffer, indices.data(), (size_t)bufferSize);
		stagingBuffer.UnmapMemory();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_INDEX_BUFFER_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		indexBuffer = std::make_shared<Vk::Buffer>(config);

		Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
			[&](VkCommandBuffer commandBuffer) -> void {
				Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), indexBuffer->Value(), bufferSize);
			}
		);
	}

	indexCount = indices.size();
	vertexCount = vertices.size();

	vertices.clear();
	vertices.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();
}

void Shape::GenerateTangents()
{
}
