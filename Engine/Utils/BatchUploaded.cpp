#include "BatchUploaded.h"
#include "Engine/Vulkan/VulkanContext.h"
#include "Engine/Vulkan/CommandBuffer.h"

void BatchUploaded::CreateCommandPoolAndBuffer()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto physicalDevice = Vk::VulkanContext::GetContext()->GetPhysicalDevice();
	auto& queueFamilyIndices = physicalDevice->GetQueueFamilyIndices();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &uploadData.commandPool), "Failed to create immediate command pool!");

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = uploadData.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &uploadData.commandBuffer), "Failed to allocate immediate command buffers!");
}

void BatchUploaded::DestoryCommandPoolAndBuffer()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();

	if (uploadData.commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(device->Value(), uploadData.commandPool, nullptr);
		uploadData.commandPool = VK_NULL_HANDLE;
	}

	uploadData.submitInfo = {};
	uploadData.commandBuffer = VK_NULL_HANDLE;
	uploadData.stagingBuffer.reset();
}

void BatchUploaded::BeginCommandBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(uploadData.commandBuffer, &beginInfo);
}

void BatchUploaded::EndCommandBuffer()
{
	vkEndCommandBuffer(uploadData.commandBuffer);
	uploadData.submitInfo = Vk::CommandBuffer::BuildSubmitInfo(uploadData.commandBuffer);
}
