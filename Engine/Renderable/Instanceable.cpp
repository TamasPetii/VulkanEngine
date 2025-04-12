#include "Instanceable.h"

void Instanceable::AddIndex(const Indices& index)
{
	instanceIndices.emplace_back(index);
	instanceCount++;
}

void Instanceable::UploadInstanceDataToGPU(uint32_t frameIndex)
{
	//Upload the vertices to the gpu
	{
		VkDeviceSize bufferSize = sizeof(Indices) * instanceIndices.size();

		Vk::BufferConfig stagingConfig;
		stagingConfig.size = bufferSize;
		stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
		stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Vk::Buffer stagingBuffer{ stagingConfig };

		void* mappedBuffer = stagingBuffer.MapMemory();
		memcpy(mappedBuffer, instanceIndices.data(), (size_t)bufferSize);
		stagingBuffer.UnmapMemory();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		instanceIndexBuffers[frameIndex] = std::make_shared<Vk::Buffer>(config);

		Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
			[&](VkCommandBuffer commandBuffer) -> void {
				Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), instanceIndexBuffers[frameIndex]->Value(), bufferSize);
			}
		);

		instanceIndices.clear();
		instanceIndices.shrink_to_fit();
	}
}

std::shared_ptr<Vk::Buffer> Instanceable::GetInstanceIndexBuffer(uint32_t frameIndex)
{
	return instanceIndexBuffers[frameIndex];
}

uint32_t Instanceable::GetInstanceCount()
{
	return instanceCount;
}

void Instanceable::ResetInstanceCount()
{
	instanceCount = 0;
}
