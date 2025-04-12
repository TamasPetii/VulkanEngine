#include "IndirectMultiDrawable.h"

void IndirectMultiDrawable::UploadIndirectCommandsToGpu()
{
	VkDeviceSize bufferSize = sizeof(VkDrawIndexedIndirectCommand) * indirectCommands.size();

	Vk::BufferConfig config;
	config.size = bufferSize;
	config.usage = VK_BUFFER_USAGE_2_INDIRECT_BUFFER_BIT;
	config.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	for (int i = 0; i < Settings::MAX_FRAMES_IN_FLIGHTS; ++i)
	{
		indirectBuffer[i] = std::make_shared<Vk::Buffer>(config);
		auto mappedBuffer = indirectBuffer[i]->MapMemory();
		memcpy(mappedBuffer, indirectCommands.data(), (size_t)bufferSize);
	}
}

void IndirectMultiDrawable::UpdateIndirectCommandsInstanceCount(uint32_t instanceCount, uint32_t frameIndex)
{
	auto mappedBuffer = static_cast<VkDrawIndexedIndirectCommand*>(indirectBuffer[frameIndex]->GetHandler());

	for (int i = 0; i < indirectCommands.size(); ++i)
		mappedBuffer->instanceCount = instanceCount;
}

std::shared_ptr<Vk::Buffer> IndirectMultiDrawable::GetIndirectBuffer(uint32_t frameIndex)
{
	return indirectBuffer[frameIndex];
}
