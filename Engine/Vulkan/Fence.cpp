#include "Fence.h"

VkFenceCreateInfo Vk::Fence::BuildCreateInfo(VkBool32 signaled)
{
	VkFenceCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	if (signaled)
		info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	return info;
}

Vk::Fence::Fence(VkBool32 signaled)
{
	Initialize(signaled);
}

Vk::Fence::~Fence()
{
	Cleanup();
}

const VkFence& Vk::Fence::Value() const
{
	return fence;
}

void Vk::Fence::ResetFence()
{
	auto device = VulkanContext::GetContext()->GetDevice();
	vkResetFences(device->Value(), 1, &fence);
}

void Vk::Fence::Initialize(VkBool32 signaled)
{
	auto device = VulkanContext::GetContext()->GetDevice();
	VkFenceCreateInfo fenceInfo = BuildCreateInfo(signaled);
	VK_CHECK_MESSAGE(vkCreateFence(device->Value(), &fenceInfo, nullptr, &fence), "Failed to create fence!");
}

void Vk::Fence::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (fence != VK_NULL_HANDLE)
	{
		vkDestroyFence(device->Value(), fence, nullptr);
		fence = VK_NULL_HANDLE;
	}
}
