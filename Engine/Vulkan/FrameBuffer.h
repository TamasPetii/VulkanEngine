#pragma once
#include "VulkanContext.h"
#include "RenderPass.h"
#include "Image.h"
#include <string>
#include <vector>
#include <span>

namespace Vk
{
	class FrameBufferBuilder;

	class ENGINE_API FrameBuffer
	{
	private:
		struct ImageData
		{
			uint32_t index;
			std::unique_ptr<Image> image;
			ImageSpecification specification;
		};
	public:
		~FrameBuffer();
		const VkFramebuffer Value() const;
		void Resize(uint32_t width, uint32_t height);
	private:
		FrameBuffer(uint32_t width, uint32_t height, std::shared_ptr<RenderPass> renderPass);
		void Init();
		void Destroy();
		void AttachImage(const std::string& imageName, uint32_t index, const ImageSpecification& specification);
		VkFramebufferCreateInfo BuildFramebufferInfo(std::span<VkImageView> imageViewAttachments);
	private:
		uint32_t width;
		uint32_t height;
		VkFramebuffer frameBuffer;
		std::shared_ptr<RenderPass> renderPass;
		std::unordered_map<std::string, ImageData> images;

		friend class FrameBufferBuilder;
	};

	class FrameBufferBuilder 
	{
	private:
		struct ImageData
		{
			uint32_t index;
			ImageSpecification specification;
		};
	public:
		void Reset();
		void SetInitialSize(uint32_t width, uint32_t height);
		void AttachImageSpec(const std::string& imageName, uint32_t index, const ImageSpecification& specification);
		void AttachDepthSpec(uint32_t index, const ImageSpecification& specification);
		std::shared_ptr<FrameBuffer> BuildFrameBuffer(std::shared_ptr<RenderPass> renderPass);
	private:
		uint32_t width;
		uint32_t height;
		std::unordered_map<std::string, ImageData> imageSpecifications;
	};
}
