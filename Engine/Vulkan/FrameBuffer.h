#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"
#include "RenderPass.h"
#include "Image.h"
#include <string>
#include <vector>
#include <span>
#include <unordered_map>

namespace Vk
{
	struct ENGINE_API FrameBufferConfig
	{
		uint32_t width{};
		uint32_t height{};
		std::optional<VkRenderPass> renderPass{};
		std::unordered_map<std::string, std::pair<uint32_t, ImageSpecification>> imageSpecifications{};
	};

	class ENGINE_API FrameBuffer
	{
	public:
		FrameBuffer(FrameBufferConfig& config);
		~FrameBuffer();

		void Resize(uint32_t width, uint32_t height);
		VkFramebuffer GetFrameBuffer() const;
		VkExtent2D GetSize() const;
		std::shared_ptr<Image> GetImage(const std::string& imageName) const;
	private:
		bool Initialize();
		void Cleanup();

		FrameBufferConfig config;
		std::vector<std::shared_ptr<Image>> images;
		VkFramebuffer frameBuffer = VK_NULL_HANDLE;
	};

	class ENGINE_API FrameBufferBuilder
	{
	public:
		FrameBufferBuilder();
		void ResetToDefault();

		FrameBufferBuilder& SetSize(uint32_t width, uint32_t height);
		FrameBufferBuilder& AddImageSpecification(const std::string& imageName, uint32_t index, const ImageSpecification& specification);
		FrameBufferBuilder& AddDepthSpecification(uint32_t index, const ImageSpecification& specification);

		std::shared_ptr<FrameBuffer> Build(VkRenderPass renderPass);
		std::shared_ptr<FrameBuffer> BuildDynamic();

	private:
		FrameBufferConfig config;
	};
}
