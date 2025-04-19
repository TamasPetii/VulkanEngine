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

void Vk::Buffer::CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height, uint32_t mipLevel)
{
	VkBufferImageCopy2 region{};
	region.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { width, height, 1 };
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = mipLevel;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	VkCopyBufferToImageInfo2 copyInfo{};
	copyInfo.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2;
	copyInfo.srcBuffer = srcBuffer;
	copyInfo.dstImage = dstImage;
	copyInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &region;

	vkCmdCopyBufferToImage2(commandBuffer, &copyInfo);
}

void Vk::Buffer::CopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkBuffer dstBuffer, uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY)
{
	VkBufferImageCopy2 region{};
	region.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2;
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.imageOffset = { (int32_t)offsetX, (int32_t)offsetY, 0 };
	region.imageExtent = { width, height, 1 };
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	VkCopyImageToBufferInfo2 copyInfo{};
	copyInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2;
	copyInfo.srcImage = srcImage;
	copyInfo.dstBuffer = dstBuffer;
	copyInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &region;

	vkCmdCopyImageToBuffer2(commandBuffer, &copyInfo);
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
	UnmapMemory();
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
	auto device = VulkanContext::GetContext()->GetDevice();
	if (bufferHandler != VK_NULL_HANDLE)
	{
		vkUnmapMemory(device->Value(), bufferMemory);
		bufferHandler = VK_NULL_HANDLE;
	}
}