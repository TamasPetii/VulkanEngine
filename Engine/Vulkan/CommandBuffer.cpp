#include "CommandBuffer.h"
#include "Engine/Vulkan/VulkanMutex.h"

VkCommandBufferSubmitInfo Vk::CommandBuffer::BuildSubmitInfo(VkCommandBuffer commandBuffer)
{
	VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
	commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmitInfo.commandBuffer = commandBuffer;
	commandBufferSubmitInfo.deviceMask = 0;
	commandBufferSubmitInfo.pNext = nullptr;

	return commandBufferSubmitInfo;
}

VkCommandBuffer Vk::CommandBuffer::BeginSingleTimeCommandBuffer(VkCommandPool commandPool)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &commandBuffer), "Failed to create single time command buffer!");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Vk::CommandBuffer::EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue queue)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	vkEndCommandBuffer(commandBuffer);

	VkCommandBufferSubmitInfo commandBufferSubmitInfo = BuildSubmitInfo(commandBuffer);

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

	std::unique_lock<std::mutex> queueLock(VulkanMutex::graphicsQueueSubmitMutex);
	vkQueueSubmit2(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
	queueLock.unlock();

	vkFreeCommandBuffers(device->Value(), commandPool, 1, &commandBuffer);
}
