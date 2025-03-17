#pragma once
#include "VulkanContext.h"
#include <string>
#include <vector>
#include <span>

namespace Vk
{
	class Image;

	class ENGINE_API Memory
	{
	public:
		static VkMemoryAllocateInfo GetImageMemoryAllocInfo(const Image* const image);
		static VkMemoryAllocateInfo GetBufferMemoryAllocInfo(VkBuffer buffer, VkMemoryPropertyFlags memoryProperties);
	private:
		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}


