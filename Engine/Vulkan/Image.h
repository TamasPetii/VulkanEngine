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
		const VkImageView Value() const;
	private:
		void Init();
		void Destroy();
		VkImageCreateInfo BuildImageInfo();
		VkImageViewCreateInfo BuildImageViewInfo();
	private:
		VkImage image;
		VkImageView imageView;
		VkDeviceMemory imageMemory;
		ImageSpecification specification;

		friend class Memory;
	};
}
