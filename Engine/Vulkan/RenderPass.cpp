#include "RenderPass.h"

VkAttachmentDescription Vk::RenderPassBuilder::BuildAttachmentDescription(VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = initialLayout;
	colorAttachment.finalLayout = finalLayout;

	return colorAttachment;
}

VkAttachmentReference Vk::RenderPassBuilder::BuildAttachmentReference(uint32_t attachment, VkImageLayout layout)
{
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = attachment;
	colorAttachmentRef.layout = layout;

	return colorAttachmentRef;
}

VkSubpassDescription Vk::RenderPassBuilder::BuildSubpassDescription(std::span<VkAttachmentReference> colorReferences, VkAttachmentReference* depthReference, std::span<VkAttachmentReference> inputReferences)
{
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
	subpass.pColorAttachments = colorReferences.empty() ? nullptr : colorReferences.data();
	subpass.inputAttachmentCount = static_cast<uint32_t>(inputReferences.size());
	subpass.pInputAttachments = inputReferences.empty() ? nullptr : inputReferences.data();
	subpass.pDepthStencilAttachment = depthReference;

	return subpass;
}

VkSubpassDependency Vk::RenderPassBuilder::BuildSubpassDependeny(const SubpassDependencyData& src, const SubpassDependencyData& dst)
{
	VkSubpassDependency dependency{};
	dependency.srcSubpass = src.subpass;
	dependency.srcStageMask = src.stageMask;
	dependency.srcAccessMask = src.accessMask;
	dependency.dstSubpass = dst.subpass;
	dependency.dstStageMask = dst.stageMask;
	dependency.dstAccessMask = dst.accessMask;

	return dependency;
}

Vk::RenderPass::RenderPass(std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subpassDescriptions, std::span<VkSubpassDependency> subpassDependencies)
{
	Init(attachmentDescriptions, subpassDescriptions, subpassDependencies);
}

Vk::RenderPass::~RenderPass()
{
	Destroy();
}

const VkRenderPass Vk::RenderPass::Value() const
{
	return renderPass;
}

void Vk::RenderPass::Init(std::span<VkAttachmentDescription> attachmentDescriptions, std::span<VkSubpassDescription> subpassDescriptions, std::span<VkSubpassDependency> subpassDependencies)
{
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.empty() ? nullptr : attachmentDescriptions.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
	renderPassInfo.pSubpasses = subpassDescriptions.empty() ? nullptr : subpassDescriptions.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());;
	renderPassInfo.pDependencies = subpassDependencies.empty() ? nullptr : subpassDependencies.data();

	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	VK_CHECK_MESSAGE(vkCreateRenderPass(device->Value(), &renderPassInfo, nullptr, &renderPass), "Failed to create render pass!");
}

void Vk::RenderPass::Destroy()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDestroyRenderPass(device->Value(), renderPass, nullptr);
}