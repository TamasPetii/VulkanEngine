#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"

namespace Vk
{
	struct ENGINE_API ImageSamplerConfig
	{
		VkFilter magFilter;
		VkFilter minFilter;
		VkSamplerMipmapMode mipMapFilter;
		VkSamplerAddressMode addressMode;
		VkBool32 anisotropyEnable;
		VkBorderColor borderColor;
	};

	class ENGINE_API ImageSampler
	{
	public:
		ImageSampler(const ImageSamplerConfig& specification);
		~ImageSampler();
		const VkSampler Value() const;
	private:
		void Init();
		void Destroy();
		ImageSamplerConfig config;
		VkSampler sampler = VK_NULL_HANDLE;
	};
}
