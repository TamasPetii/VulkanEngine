#include "ImmediateQueue.h"
#include "VulkanMutex.h"

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

void Vk::ImmediateQueue::SubmitGraphics(const std::function<void(VkCommandBuffer)>& function)
{
	std::unique_lock<std::mutex> submitLock(graphicsMutex);

	VK_CHECK_MESSAGE(vkResetFences(device->Value(), 1, &graphicsFence), "Couldn't reset immediate fence");
	VK_CHECK_MESSAGE(vkResetCommandBuffer(graphicsCommandBuffer, 0), "Couldn't reset immediate fence");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(graphicsCommandBuffer, &beginInfo), "Failed to begin recording command buffer!");

	function(graphicsCommandBuffer);

	VK_CHECK_MESSAGE(vkEndCommandBuffer(graphicsCommandBuffer), "Failed to end command buffer!");

	VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
	commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmitInfo.commandBuffer = graphicsCommandBuffer;
	commandBufferSubmitInfo.deviceMask = 0;
	commandBufferSubmitInfo.pNext = nullptr;

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

	std::unique_lock<std::mutex> queueLock(VulkanMutex::graphicsQueueSubmitMutex);
	VK_CHECK_MESSAGE(vkQueueSubmit2(device->GetQueue(QueueType::GRAPHICS), 1, &submitInfo, graphicsFence), "Failed to submit immediate command to queue!");
	queueLock.unlock();

	VK_CHECK_MESSAGE(vkWaitForFences(device->Value(), 1, &graphicsFence, true, UINT64_MAX), "Failed to wait for immediate fence");
}

void Vk::ImmediateQueue::SubmitGraphics(std::span<VkCommandBufferSubmitInfo> commandBufferSubmitInfos)
{
	if (commandBufferSubmitInfos.size() == 0)
		return;

	std::unique_lock<std::mutex> submitLock(graphicsMutex);

	VK_CHECK_MESSAGE(vkResetFences(device->Value(), 1, &graphicsFence), "Couldn't reset immediate fence");

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferInfoCount = static_cast<uint32_t>(commandBufferSubmitInfos.size());
	submitInfo.pCommandBufferInfos = commandBufferSubmitInfos.data();

	std::unique_lock<std::mutex> queueLock(VulkanMutex::graphicsQueueSubmitMutex);
	VK_CHECK_MESSAGE(vkQueueSubmit2(device->GetQueue(QueueType::GRAPHICS), 1, &submitInfo, graphicsFence), "Failed to submit immediate command to queue!");
	queueLock.unlock();

	VK_CHECK_MESSAGE(vkWaitForFences(device->Value(), 1, &graphicsFence, true, UINT64_MAX), "Failed to wait for immediate fence");
}

void Vk::ImmediateQueue::SubmitTransfer(const std::function<void(VkCommandBuffer)>& function)
{
	std::unique_lock<std::mutex> submitLock(transferMutex);

	VK_CHECK_MESSAGE(vkResetFences(device->Value(), 1, &transferFence), "Couldn't reset immediate fence");
	VK_CHECK_MESSAGE(vkResetCommandBuffer(transferCommandBuffer, 0), "Couldn't reset immediate fence");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	beginInfo.pInheritanceInfo = nullptr;

	VK_CHECK_MESSAGE(vkBeginCommandBuffer(transferCommandBuffer, &beginInfo), "Failed to begin recording command buffer!");

	function(transferCommandBuffer);

	VK_CHECK_MESSAGE(vkEndCommandBuffer(transferCommandBuffer), "Failed to end command buffer!");

	VkCommandBufferSubmitInfo commandBufferSubmitInfo{};
	commandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmitInfo.commandBuffer = transferCommandBuffer;
	commandBufferSubmitInfo.deviceMask = 0;
	commandBufferSubmitInfo.pNext = nullptr;

	VkSubmitInfo2 submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	submitInfo.pNext = nullptr;
	submitInfo.commandBufferInfoCount = 1;
	submitInfo.pCommandBufferInfos = &commandBufferSubmitInfo;

	std::unique_lock<std::mutex> queueLock(VulkanMutex::transferQueueSubmitMutex);
	VK_CHECK_MESSAGE(vkQueueSubmit2(device->GetQueue(QueueType::TRANSFER), 1, &submitInfo, transferFence), "Failed to submit immediate command to queue!");
	queueLock.unlock();

	VK_CHECK_MESSAGE(vkWaitForFences(device->Value(), 1, &transferFence, true, UINT64_MAX), "Failed to wait for immediate fence");
}

void Vk::ImmediateQueue::Initialize()
{
	auto& queueFamilyIndices = physicalDevice->GetQueueFamilyIndices();

	InitResource(graphicsFence, graphicsCommandPool, graphicsCommandBuffer, queueFamilyIndices.graphicsFamily.value());
	InitResource(transferFence, transferCommandPool, transferCommandBuffer, queueFamilyIndices.transferFamily.value());
}

void Vk::ImmediateQueue::Cleanup()
{
	if (graphicsCommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device->Value(), graphicsCommandPool, nullptr);
		graphicsCommandPool = VK_NULL_HANDLE;
	}

	if (graphicsFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device->Value(), graphicsFence, nullptr);
		graphicsFence = VK_NULL_HANDLE;
	}

	if (transferCommandPool != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device->Value(), transferCommandPool, nullptr);
		transferCommandPool = VK_NULL_HANDLE;
	}

	if (transferFence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device->Value(), transferFence, nullptr);
		transferFence = VK_NULL_HANDLE;
	}
}

void Vk::ImmediateQueue::InitResource(VkFence& fence, VkCommandPool& commandPool, VkCommandBuffer& commandBuffer, uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndex;

	VK_CHECK_MESSAGE(vkCreateCommandPool(device->Value(), &poolInfo, nullptr, &commandPool), "Failed to create immediate command pool!");

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VK_CHECK_MESSAGE(vkAllocateCommandBuffers(device->Value(), &allocInfo, &commandBuffer), "Failed to allocate immediate command buffers!");

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK_MESSAGE(vkCreateFence(device->Value(), &fenceInfo, nullptr, &fence), "Failed to create fence!");
}
