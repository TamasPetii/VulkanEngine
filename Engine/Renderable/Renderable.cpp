#include "Renderable.h"

void Renderable::UploadToGpu()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	//Vertex Buffer
	VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();

	Vk::BufferConfig vertexStagingConfig;
	vertexStagingConfig.size = vertexBufferSize;
	vertexStagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
	vertexStagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	Vk::Buffer vertexStagingBuffer{ vertexStagingConfig };

	void* vertexStagigngBufferHandler = vertexStagingBuffer.MapMemory();
	memcpy(vertexStagigngBufferHandler, vertices.data(), (size_t)vertexBufferSize);
	vertexStagingBuffer.UnmapMemory();

	Vk::BufferConfig vertexBufferConfig;
	vertexBufferConfig.size = vertexBufferSize;
	vertexBufferConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
	vertexBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	vertexBuffer = std::make_shared<Vk::Buffer>(vertexBufferConfig);

	//Index Buffer
	VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();

	Vk::BufferConfig indexStagingConfig;
	indexStagingConfig.size = indexBufferSize;
	indexStagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
	indexStagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	Vk::Buffer indexStagingBuffer{ indexStagingConfig };

	void* indexStagingBufferHandler = indexStagingBuffer.MapMemory();
	memcpy(indexStagingBufferHandler, indices.data(), (size_t)indexBufferSize);
	indexStagingBuffer.UnmapMemory();

	Vk::BufferConfig indexBufferConfig;
	indexBufferConfig.size = indexBufferSize;
	indexBufferConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_INDEX_BUFFER_BIT;
	indexBufferConfig.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	indexBuffer = std::make_shared<Vk::Buffer>(indexBufferConfig);

	Vk::VulkanContext::GetContext()->GetImmediateQueue()->SubmitTransfer(
		[&](VkCommandBuffer commandBuffer) -> void {
			Vk::Buffer::CopyBufferToBuffer(commandBuffer, vertexStagingBuffer.Value(), vertexBuffer->Value(), vertexBufferSize);
			Vk::Buffer::CopyBufferToBuffer(commandBuffer, indexStagingBuffer.Value(), indexBuffer->Value(), indexBufferSize);
		}
	);

	indexCount = indices.size();
	indices.clear();
	indices.shrink_to_fit();

	vertexCount = vertices.size();
	vertices.clear();
	vertices.shrink_to_fit();
}
