#include "Buffer.h"
#include "Memory.h"

void Vk::Buffer::CopyBufferToBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;

	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
}

Vk::Buffer::Buffer(const BufferConfig& config) :
	config(config)
{
	Init();
}

Vk::Buffer::~Buffer()
{
	Destroy();
}

void Vk::Buffer::Init()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = config.size;
	bufferInfo.usage = config.usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK_MESSAGE(vkCreateBuffer(device->Value(), &bufferInfo, nullptr, &buffer), "Failed to create buffer!");

	VkMemoryAllocateInfo allocInfo = Memory::GetBufferMemoryAllocInfo(buffer, config.memoryProperties);
	
	VkMemoryAllocateFlagsInfo allocFlagsInfo{};
	allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;

	if (config.usage & VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT)
	{
		allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
		allocInfo.pNext = &allocFlagsInfo;
	}
	
	VK_CHECK_MESSAGE(vkAllocateMemory(device->Value(), &allocInfo, nullptr, &bufferMemory), "Failed to allocate buffer memory!");

	vkBindBufferMemory(device->Value(), buffer, bufferMemory, 0);

	if (config.usage & VK_BUFFER_USAGE_2_SHADER_DEVICE_ADDRESS_BIT)
	{
		VkBufferDeviceAddressInfo deviceAdressInfo{};
		deviceAdressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		deviceAdressInfo.buffer = buffer;

		bufferAddress = vkGetBufferDeviceAddress(device->Value(), &deviceAdressInfo);
	}
}

void Vk::Buffer::Destroy()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	vkDestroyBuffer(device->Value(), buffer, nullptr);
	vkFreeMemory(device->Value(), bufferMemory, nullptr);
}

const VkBuffer Vk::Buffer::Value() const
{
	return buffer;
}

const VkDeviceAddress& Vk::Buffer::GetAddress() const
{
	return bufferAddress;
}

void* Vk::Buffer::MapMemory()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (config.memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		vkMapMemory(device->Value(), bufferMemory, 0, config.size, 0, &bufferHandler);
		return bufferHandler;
	}

	return nullptr;
}
void Vk::Buffer::UnmapMemory()
{
	if (bufferHandler != VK_NULL_HANDLE)
	{
		auto device = VulkanContext::GetContext()->GetDevice();
		vkUnmapMemory(device->Value(), bufferMemory);
	}
}