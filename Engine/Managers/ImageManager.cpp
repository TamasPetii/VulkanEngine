#include "ImageManager.h"
#include <stb_image.h>
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"

ImageManager::ImageManager(std::shared_ptr<VulkanManager> vulkanManager) :
	vulkanManager(vulkanManager)
{
	Initialize();
}

ImageManager::~ImageManager()
{
	Cleanup();
}

void ImageManager::Initialize()
{
}

void ImageManager::Cleanup()
{
	images.clear();
}

uint32_t ImageManager::GetAvailableIndex()
{
	if (availableIndices.empty())
		return counter++;

	uint32_t index = *availableIndices.begin();
	availableIndices.erase(index);
	return index;
}

std::shared_ptr<Vk::Image> ImageManager::GetImage(const std::string& path)
{
	if (images.find(path) == images.end())
		return nullptr;

	return images.at(path);
}

std::shared_ptr<Vk::Image> ImageManager::LoadImage(const std::string& path, bool useMipMap)
{
	if (images.find(path) != images.end())
		return images.at(path);

	int texWidth, texHeight, texChannels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels)
		return nullptr;

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	Vk::BufferConfig stagingConfig;
	stagingConfig.size = imageSize;
	stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
	stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	Vk::Buffer stagingBuffer{ stagingConfig };

	void* mappedBuffer = stagingBuffer.MapMemory();
	memcpy(mappedBuffer, pixels, (size_t)imageSize);
	stagingBuffer.UnmapMemory();

	stbi_image_free(pixels);

	Vk::ImageSpecification imageSpec;
	imageSpec.width = static_cast<uint32_t>(texWidth);
	imageSpec.height = static_cast<uint32_t>(texHeight);
	imageSpec.type = VK_IMAGE_TYPE_2D;
	imageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageSpec.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageSpec.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (useMipMap && Vk::Image::ImageFormatSupportsLinearMipMap(imageSpec.format))
		imageSpec.mipmapLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	std::shared_ptr<Vk::Image> image = std::make_shared<Vk::Image>(imageSpec, GetAvailableIndex());

	Vk::VulkanContext::GetContext()->GetImmediateQueue()->Submit(
		[&](VkCommandBuffer commandBuffer) -> void {
			Vk::Image::TransitionImageLayoutDynamic(commandBuffer, image->Value(),
				VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, imageSpec.mipmapLevel);

			Vk::Buffer::CopyBufferToImage(commandBuffer, stagingBuffer.Value(), image->Value(), imageSpec.width, imageSpec.height);

			if (imageSpec.mipmapLevel > 1)
			{
				Vk::Image::GenerateMipMaps(commandBuffer, image->Value(), imageSpec.width, imageSpec.height, imageSpec.mipmapLevel);
			}
			else
			{
				Vk::Image::TransitionImageLayoutDynamic(commandBuffer, image->Value(),
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_READ_BIT);
			}
		}
	);

	images[path] = image;
	vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", images[path]->GetImageView(), VK_NULL_HANDLE, images[path]->GetDescriptorArrayIndex());
	return images.at(path);
}