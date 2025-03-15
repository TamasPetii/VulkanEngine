#include "Image.h"
#include "Memory.h"

Vk::Image::Image(const ImageSpecification& specification) :
	specification(specification)
{
	Init();
}

Vk::Image::~Image()
{
	Destroy();
}

const VkImageView Vk::Image::Value() const
{
	return imageView;
}

void Vk::Image::Init()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	auto imageInfo = BuildImageInfo();
	VK_CHECK_MESSAGE(vkCreateImage(device->Value(), &imageInfo, nullptr, &image), "Failed to create image!");

	auto allocInfo = Memory::GetImageMemoryAllocInfo(this);
	VK_CHECK_MESSAGE(vkAllocateMemory(device->Value(), &allocInfo, nullptr, &imageMemory), "Failed to allocate image memory!");

	vkBindImageMemory(device->Value(), image, imageMemory, 0);

	auto imageViewInfo = BuildImageViewInfo();
	VK_CHECK_MESSAGE(vkCreateImageView(device->Value(), &imageViewInfo, nullptr, &imageView), "Failed to create texture image view!");
}

void Vk::Image::Destroy()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if(imageView != VK_NULL_HANDLE)
		vkDestroyImageView(device->Value(), imageView, nullptr);
	
	if(image != VK_NULL_HANDLE)
		vkDestroyImage(device->Value(), image, nullptr);
	
	if(imageMemory != VK_NULL_HANDLE)
		vkFreeMemory(device->Value(), imageMemory, nullptr);

	image = VK_NULL_HANDLE;
	imageView = VK_NULL_HANDLE;
	imageMemory = VK_NULL_HANDLE;
}

VkImageCreateInfo Vk::Image::BuildImageInfo()
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = specification.type;
	imageInfo.extent.width = specification.width;
	imageInfo.extent.height = specification.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = specification.format;
	imageInfo.tiling = specification.tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = specification.usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	return imageInfo;
}

VkImageViewCreateInfo Vk::Image::BuildImageViewInfo()
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = specification.viewType;
	viewInfo.format = specification.format;
	viewInfo.subresourceRange.aspectMask = specification.aspectFlag;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	return viewInfo;
}
