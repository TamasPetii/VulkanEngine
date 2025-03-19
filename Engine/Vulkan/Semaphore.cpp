#include "Semaphore.h"

VkSemaphoreCreateInfo Vk::Semaphore::BuildCreateInfo()
{
	VkSemaphoreCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	return info;
}

Vk::Semaphore::Semaphore()
{
	Initialize();
}

Vk::Semaphore::~Semaphore()
{
	Cleanup();
}

const VkSemaphore& Vk::Semaphore::Value() const
{
	return semaphore;
}

void Vk::Semaphore::Initialize()
{
	auto device = VulkanContext::GetContext()->GetDevice();
	VkSemaphoreCreateInfo semaphoreInfo = BuildCreateInfo();
	VK_CHECK_MESSAGE(vkCreateSemaphore(device->Value(), &semaphoreInfo, nullptr, &semaphore), "Failed to create semaphore!");
}

void Vk::Semaphore::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (semaphore != VK_NULL_HANDLE)
	{
		vkDestroySemaphore(device->Value(), semaphore, nullptr);
		semaphore = VK_NULL_HANDLE;
	}
}
