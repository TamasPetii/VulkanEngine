#pragma once
#include "VulkanContext.h"
#include <string>
#include <vector>
#include <span>
#include <unordered_map>
#include <unordered_set>

namespace Vk
{
	class RenderPassBuilder;

	enum class ENGINE_API DependencyStage
	{
		SRC,
		DST
	};

	class ENGINE_API RenderPass
	{
	public:
		~RenderPass();
		RenderPass(std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subpassDescriptions, std::span<VkSubpassDependency> subpassDependencies);
		const VkRenderPass Value() const;
	private:
		void Init(std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subpassDescriptions, std::span<VkSubpassDependency> subpassDependencies);
		void Destroy();
	private:
		VkRenderPass renderPass = VK_NULL_HANDLE;
		friend class RenderPassBuilder;
	};

	class ENGINE_API RenderPassBuilder
	{
	private:

		struct SubpassDependencyData
		{
			uint32_t subpass;
			VkPipelineStageFlags stageMask;
			VkAccessFlags accessMask;
		};

		struct SubpassData
		{
			uint32_t index;
			SubpassDependencyData dependencySrc;
			SubpassDependencyData dependencyDst;
			std::vector<VkAttachmentReference> colorRefences;
			std::vector<VkAttachmentReference> inputRefences;
			VkAttachmentReference depthReference;
		};

		struct ImageData
		{
			uint32_t index;
			VkAttachmentDescription description;
		};

	public:
		void Reset();
		void RegisterSubpass(const std::string& name, uint32_t index);
		void AttachDepthDescription(uint32_t index, VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout);
		void AttachImageDescription(const std::string& imageName, uint32_t index, VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout);
		void AttachImageReferenceToSubpass(const std::string& subpassName, const std::string& imageName, VkImageLayout layout);
		void AttachInputReferenceToSubpass(const std::string& subpassName, const std::string& imageName, VkImageLayout layout);
		void AttachDepthReferenceToSubpass(const std::string& subpassName, VkImageLayout layout);
		void AttachSubpassDependency(const std::string& subpassName, DependencyStage type, uint32_t subpassIndex, VkPipelineStageFlags stageMask, VkAccessFlags accessMask);
		std::shared_ptr<RenderPass> BuildRenderPass();
	private:
		VkAttachmentDescription BuildAttachmentDescription(VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout);
		VkAttachmentReference BuildAttachmentReference(uint32_t attachment, VkImageLayout layout);
		VkSubpassDescription BuildSubpassDescription(std::span<VkAttachmentReference> colorReferences, VkAttachmentReference* depthReference = nullptr, std::span<VkAttachmentReference> inputReferences = {});
		VkSubpassDependency BuildSubpassDependeny(const SubpassDependencyData& src, const SubpassDependencyData& dst);
		bool ValidImageName(const std::string& imageName);
		bool ValidSubpassName(const std::string& subpassName);
	private:
		std::unordered_map<std::string, ImageData> attachmentDescriptions;
		std::unordered_map<std::string, SubpassData> subpassReferences;
	};
}
