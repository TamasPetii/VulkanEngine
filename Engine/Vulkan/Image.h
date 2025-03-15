#pragma once
#include "VulkanContext.h"
#include <string>
#include <vector>
#include <span>

namespace Vk
{
	class Memory;

	struct ENGINE_API ImageSpecification
	{
		uint32_t width;
		uint32_t height;
		VkImageType type;
		VkFormat format;
		VkImageTiling tiling;
		VkImageUsageFlags usage;
		VkImageViewType viewType;
		VkImageAspectFlagBits aspectFlag;
		VkMemoryPropertyFlags memoryProperties;
	};

	class ENGINE_API Image
	{
	public:
		Image(const ImageSpecification& specification);
		~Image();
		void Init();
		void Destroy();
		const VkImage Value() const;
		const VkImageView GetImageView() const;
		static void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout srcLayout, VkPipelineStageFlags srcStageFlags, VkAccessFlags srcAccessMask, VkImageLayout dstLayout, VkPipelineStageFlags dstStageFlags, VkAccessFlags dstAccessMask);
		static void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkExtent2D srcSize, VkImage dstImage, VkExtent2D dstSize);
	private:
		VkImageCreateInfo BuildImageInfo();
		VkImageViewCreateInfo BuildImageViewInfo();
	private:
		VkImage image = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
		VkDeviceMemory imageMemory = VK_NULL_HANDLE;
		ImageSpecification specification;

		friend class Memory;
	};
}
