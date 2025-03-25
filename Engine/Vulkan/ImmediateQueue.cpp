#include "ImmediateQueue.h"

Vk::ImmediateQueue::ImmediateQueue(const PhysicalDevice* const physicalDevice, const Device* const device) :
	physicalDevice(physicalDevice),
	device(device)
{
	Initialize();
}

Vk::ImmediateQueue::~ImmediateQueue()
{
	Cleanup();
}

void Vk::ImmediateQueue::Submit(const std::function<void(VkCommandBuffer)>& function)
{
	VK_CHECK_MESSAGE(vkResetFences(device->Value(), 1, &immediateFence), "Couldn't reset immediate fence");
	VK_CHECK_MESSAGE(vkResetCommandBuffer(immediateBuffer, 0), "Couldn't reset immediate fence");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(immediateBuffer, &beginInfo), "Failed to begin recording command buffer!");

	function(immediateBuffer);

	VK_CHECK_MESSAGE(vkEndCommandBuffer(immediateBuffer), "Failed to end command buffer!");

	VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
	commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmitInfo.commandBuffer = immediateBuffer;
	commandBufferSubmitInfo.deviceMask = 0;
	commandBufferSubmitInfo.pNext = nullptr;

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

	VK_CHECK_MESSAGE(vkQueueSubmit2(device->GetQueue(QueueType::GRAPHICS), 1, &submitInfo, immediateFence), "Failed to submit immediate command to queue!");
	VK_CHECK_MESSAGE(vkWaitForFences(device->Value(), 1, &immediateFence, true, UINT64_MAX), "Failed to wait for immediate fence");
}

void Vk::ImmediateQueue::Initialize()
{
	auto& queueFamilyIndices = physicalDevice->GetQueueFamilyIndices();

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &immediatePool), "Failed to create immediate command pool!");

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = immediatePool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &immediateBuffer), "Failed to allocate immediate command buffers!");

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK_MESSAGE(vkCreateFence(device->Value(), &fenceInfo, nullptr, &immediateFence), "Failed to create fence!");
}

void Vk::ImmediateQueue::Cleanup()
{
	if (immediatePool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device->Value(), immediatePool, nullptr);
		immediatePool = VK_NULL_HANDLE;
	}

	if (immediateFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device->Value(), immediateFence, nullptr);
		immediateFence = VK_NULL_HANDLE;
	}
}
