#include "Instanceable.h"

void Instanceable::AddIndex(const uint32_t index)
{
	instanceIndices.push_back(index);
	instanceCount++;
}

void Instanceable::UploadInstanceDataToGPU(uint32_t frameIndex)
{
	//Upload the vertices to the gpu
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * instanceIndices.size();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

		instanceIndexBuffers[frameIndex] = std::make_shared<Vk::Buffer>(config);
		auto mappedBuffer = instanceIndexBuffers[frameIndex]->MapMemory();
		memcpy(mappedBuffer, instanceIndices.data(), (size_t)bufferSize);
		instanceIndexBuffers[frameIndex]->UnmapMemory();

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
