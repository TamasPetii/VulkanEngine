#include "Memory.h"
#include "Image.h"

VkMemoryAllocateInfo Vk::Memory::GetImageMemoryAllocInfo(const Image* const image)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device->Value(), image->image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, image->specification.memoryProperties);

	return allocInfo;
}

VkMemoryAllocateInfo Vk::Memory::GetBufferMemoryAllocInfo(VkBuffer buffer, VkMemoryPropertyFlags memoryProperties)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device->Value(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);

	return allocInfo;
}

uint32_t Vk::Memory::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	auto physicalDevice = VulkanContext::GetContext()->GetPhysicalDevice();

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice->Value(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
