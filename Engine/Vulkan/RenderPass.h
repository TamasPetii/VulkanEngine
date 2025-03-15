#pragma once
#include "VulkanContext.h"
#include <string>
#include <vector>
#include <span>

namespace Vk
{
	class ENGINE_API RenderPass
	{
	public:
		RenderPass(std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subpassDescriptions, std::span<VkSubpassDependency> subpassDependencies);
		~RenderPass();
		const VkRenderPass Value() const;
	private:
		void Init(std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subpassDescriptions, std::span<VkSubpassDependency> subpassDependencies);
		void Destroy();
	private:
		VkRenderPass renderPass;
	};

	struct ENGINE_API SubpassDependencyData
	{
		uint32_t subpass;
		VkPipelineStageFlags stageMask;
		VkAccessFlags accessMask;
	};

	class ENGINE_API RenderPassBuilder
	{
	public:
		static VkAttachmentDescription BuildAttachmentDescription(VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout);
		static VkAttachmentReference BuildAttachmentReference(uint32_t attachment, VkImageLayout layout);
		static VkSubpassDescription BuildSubpassDescription(std::span<VkAttachmentReference> colorReferences, VkAttachmentReference* depthReference = nullptr, std::span<VkAttachmentReference> inputReferences = {});
		static VkSubpassDependency BuildSubpassDependeny(const SubpassDependencyData& src, const SubpassDependencyData& dst);
	};
}
