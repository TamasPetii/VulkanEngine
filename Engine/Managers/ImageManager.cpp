#include "ImageManager.h"
#include <stb_image.h>
#include <gli/gli.hpp>
#include <filesystem>
#include "Engine/Vulkan/Buffer.h"
#include "Engine/Vulkan/VulkanContext.h"

VkFormat GliFormatToVulkan(gli::format format)
{
	static const std::unordered_map<gli::format, VkFormat> formatMap = {
		// BC (DXT) compressed formats
		{gli::FORMAT_RGB_DXT1_UNORM_BLOCK8, VK_FORMAT_BC1_RGB_UNORM_BLOCK},
		{gli::FORMAT_RGBA_DXT1_UNORM_BLOCK8, VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
		{gli::FORMAT_RGBA_DXT3_UNORM_BLOCK16, VK_FORMAT_BC2_UNORM_BLOCK},
		{gli::FORMAT_RGBA_DXT5_UNORM_BLOCK16, VK_FORMAT_BC3_UNORM_BLOCK},
		{gli::FORMAT_R_ATI1N_UNORM_BLOCK8, VK_FORMAT_BC4_UNORM_BLOCK},
		{gli::FORMAT_RG_ATI2N_UNORM_BLOCK16, VK_FORMAT_BC5_UNORM_BLOCK},
		{gli::FORMAT_RGBA_BP_UNORM_BLOCK16, VK_FORMAT_BC7_UNORM_BLOCK},

		// Uncompressed formats
		{gli::FORMAT_RGBA8_UNORM_PACK8, VK_FORMAT_R8G8B8A8_UNORM},
		{gli::FORMAT_RGB8_UNORM_PACK8, VK_FORMAT_R8G8B8_UNORM},
		{gli::FORMAT_R8_UNORM_PACK8, VK_FORMAT_R8_UNORM},
		{gli::FORMAT_RG8_UNORM_PACK8, VK_FORMAT_R8G8_UNORM},
		{gli::FORMAT_RGBA16_UNORM_PACK16, VK_FORMAT_R16G16B16A16_UNORM},
		{gli::FORMAT_RGB16_UNORM_PACK16, VK_FORMAT_R16G16B16_UNORM},
		{gli::FORMAT_R16_UNORM_PACK16, VK_FORMAT_R16_UNORM},

		// SRGB formats
		{gli::FORMAT_RGB_DXT1_SRGB_BLOCK8, VK_FORMAT_BC1_RGB_SRGB_BLOCK},
		{gli::FORMAT_RGBA_DXT1_SRGB_BLOCK8, VK_FORMAT_BC1_RGBA_SRGB_BLOCK},
		{gli::FORMAT_RGBA_DXT3_SRGB_BLOCK16, VK_FORMAT_BC2_SRGB_BLOCK},
		{gli::FORMAT_RGBA_DXT5_SRGB_BLOCK16, VK_FORMAT_BC3_SRGB_BLOCK},
		{gli::FORMAT_RGBA_BP_SRGB_BLOCK16, VK_FORMAT_BC7_SRGB_BLOCK},
		{gli::FORMAT_RGBA8_SRGB_PACK8, VK_FORMAT_R8G8B8A8_SRGB},
		{gli::FORMAT_RGB8_SRGB_PACK8, VK_FORMAT_R8G8B8_SRGB},

		// Floating-point formats
		{gli::FORMAT_RGBA16_SFLOAT_PACK16, VK_FORMAT_R16G16B16A16_SFLOAT},
		{gli::FORMAT_RGB16_SFLOAT_PACK16, VK_FORMAT_R16G16B16_SFLOAT},
		{gli::FORMAT_R16_SFLOAT_PACK16, VK_FORMAT_R16_SFLOAT},
		{gli::FORMAT_RGBA32_SFLOAT_PACK32, VK_FORMAT_R32G32B32A32_SFLOAT},
		{gli::FORMAT_RGB32_SFLOAT_PACK32, VK_FORMAT_R32G32B32_SFLOAT},
		{gli::FORMAT_R32_SFLOAT_PACK32, VK_FORMAT_R32_SFLOAT},
	};

	auto it = formatMap.find(format);
	if (it != formatMap.end()) {
		return it->second;
	}

	return VK_FORMAT_UNDEFINED;
}

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

	std::string extension = std::filesystem::path(path).extension().string();

	std::shared_ptr<Vk::Image> image = nullptr;

	if (extension == ".dds")
	{
		gli::texture texture = gli::load(path);

		if (!texture.empty() && texture.target() == gli::target::TARGET_2D && texture.layers() == 1)
			image = CreateVulkanImage(GliFormatToVulkan(texture.format()), texture.size(0), texture.extent().x, texture.extent().y, false, texture.data(0, 0, 0));
	}
	else
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!pixels)
			return nullptr;

		image = CreateVulkanImage(VK_FORMAT_R8G8B8A8_UNORM, texWidth * texHeight * 4, texWidth, texHeight, useMipMap, pixels);

		stbi_image_free(pixels);
	}

	if (!image)
		return nullptr;

	images[path] = image;
	vulkanManager->GetDescriptorSet("LoadedImages")->UpdateImageArrayElement("Images", images[path]->GetImageView(), VK_NULL_HANDLE, images[path]->GetDescriptorArrayIndex());
	return images.at(path);
}

std::shared_ptr<Vk::Image> ImageManager::CreateVulkanImage(VkFormat format, VkDeviceSize size, int width, int height, bool useMipMap, void* data)
{
	Vk::BufferConfig stagingConfig;
	stagingConfig.size = size;
	stagingConfig.usage = VK_BUFFER_USAGE_2_TRANSFER_SRC_BIT;
	stagingConfig.memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	Vk::Buffer stagingBuffer{ stagingConfig };

	void* mappedBuffer = stagingBuffer.MapMemory();
	memcpy(mappedBuffer, data, (size_t)size);
	stagingBuffer.UnmapMemory();

	Vk::ImageSpecification imageSpec;
	imageSpec.width = static_cast<uint32_t>(width);
	imageSpec.height = static_cast<uint32_t>(height);
	imageSpec.type = VK_IMAGE_TYPE_2D;
	imageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageSpec.format = format;
	imageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageSpec.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (useMipMap && Vk::Image::ImageFormatSupportsLinearMipMap(imageSpec.format))
		imageSpec.mipmapLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

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

	return image;
}