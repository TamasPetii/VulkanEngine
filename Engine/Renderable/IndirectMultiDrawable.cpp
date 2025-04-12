#include "IndirectMultiDrawable.h"

void IndirectMultiDrawable::UploadIndirectCommandsToGpu(uint32_t frameIndex)
{
    //Upload the indirect commands to the gpu
    {
		VkDeviceSize bufferSize = sizeof(VkDrawIndexedIndirectCommand) * indirectCommands.size();

		Vk::BufferConfig stagingConfig;
		stagingConfig.size = bufferSize;
		stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
		stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Vk::Buffer stagingBuffer{ stagingConfig };

		void* mappedBuffer = stagingBuffer.MapMemory();
		memcpy(mappedBuffer, indirectCommands.data(), (size_t)bufferSize);
		stagingBuffer.UnmapMemory();

		Vk::BufferConfig config;
		config.size = bufferSize;
		config.usage = VK_BUFFER_USAGE_2_TRANSFER_DST_BIT | VK_BUFFER_USAGE_2_INDIRECT_BUFFER_BIT;
		config.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		indirectBuffer[frameIndex] = std::make_shared<Vk::Buffer>(config);

		Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
			[&](VkCommandBuffer commandBuffer) -> void {
				Vk::Buffer::CopyBufferToBuffer(commandBuffer, stagingBuffer.Value(), indirectBuffer[frameIndex]->Value(), bufferSize);
			}
		);
    }
}

void IndirectMultiDrawable::UpdateIndirectCommandsInstanceCount(uint32_t instanceCount)
{
	for (auto& indirectCommand : indirectCommands)
		indirectCommand.instanceCount = instanceCount;
}

std::shared_ptr<Vk::Buffer> IndirectMultiDrawable::GetIndirectBuffer(uint32_t frameIndex)
{
	return indirectBuffer[frameIndex];
}
