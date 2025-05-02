#pragma once
#include "VulkanContext.h"
#include <string>
#include <vector>
#include <span>

namespace Vk
{
	class Memory;

	struct ENGINE_API BufferConfig
	{
		VkDeviceSize size;
		VkBufferUsageFlags2 usage;
		VkMemoryPropertyFlags memoryProperties;
	};

	class ENGINE_API Buffer
	{
	public:
		static void CopyBufferToBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		static void CopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height, uint32_t mipLevel = 0, VkDeviceSize bufferOffset = 0);
		static void CopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkBuffer dstBuffer, uint32_t width, uint32_t height, uint32_t offsetX, uint32_t offsetY);

		Buffer(const BufferConfig& config);
		~Buffer();
		void Init();
		void Destroy();
		const VkBuffer Value() const;
		const VkDeviceAddress& GetAddress() const;
		void* GetHandler() { return bufferHandler; }
		void* MapMemory();
		void UnmapMemory();
	private:
		VkDeviceAddress bufferAddress;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		void* bufferHandler = VK_NULL_HANDLE;

		BufferConfig config;
		friend class Memory;
	};
}
