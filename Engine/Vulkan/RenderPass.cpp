#include "RenderPass.h"

VkAttachmentDescription Vk::RenderPassBuilder::BuildAttachmentDescription(VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	VkAttachmentDescription imageAttachment{};
	imageAttachment.format = format;
	imageAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	imageAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	imageAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	imageAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	imageAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	imageAttachment.initialLayout = initialLayout;
	imageAttachment.finalLayout = finalLayout;

	return imageAttachment;
}

VkAttachmentReference Vk::RenderPassBuilder::BuildAttachmentReference(uint32_t attachment, VkImageLayout layout)
{
	VkAttachmentReference imageAttachmentRef{};
	imageAttachmentRef.attachment = attachment;
	imageAttachmentRef.layout = layout;

	return imageAttachmentRef;
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

bool Vk::RenderPassBuilder::ValidImageName(const std::string& imageName)
{
	return attachmentDescriptions.find(imageName) != attachmentDescriptions.end();
}

bool Vk::RenderPassBuilder::ValidSubpassName(const std::string& subpassName)
{
	return subpassReferences.find(subpassName) != subpassReferences.end();
}

void Vk::RenderPassBuilder::Reset()
{
	attachmentDescriptions.clear();
	subpassReferences.clear();
}

void Vk::RenderPassBuilder::RegisterSubpass(const std::string& subpassName, uint32_t index)
{
	subpassReferences[subpassName].index = index;
}

void Vk::RenderPassBuilder::AttachDepthDescription(uint32_t index, VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	AttachImageDescription("Depth", index, format, initialLayout, finalLayout);
}

void Vk::RenderPassBuilder::AttachImageDescription(const std::string& imageName, uint32_t index, VkFormat format, VkImageLayout initialLayout, VkImageLayout finalLayout)
{
	auto imageDesciption = BuildAttachmentDescription(format, initialLayout, finalLayout);
	attachmentDescriptions[imageName].index = index;
	attachmentDescriptions[imageName].description = imageDesciption;
}

void Vk::RenderPassBuilder::AttachImageReferenceToSubpass(const std::string& subpassName, const std::string& imageName, VkImageLayout layout)
{
	auto imageReference = BuildAttachmentReference(attachmentDescriptions[imageName].index, layout);
	subpassReferences[subpassName].colorRefences.push_back(imageReference);
}

void Vk::RenderPassBuilder::AttachInputReferenceToSubpass(const std::string& subpassName, const std::string& imageName, VkImageLayout layout)
{
	auto imageReference = BuildAttachmentReference(attachmentDescriptions[imageName].index, layout);
	subpassReferences[subpassName].inputRefences.push_back(imageReference);
}

void Vk::RenderPassBuilder::AttachDepthReferenceToSubpass(const std::string& subpassName, VkImageLayout layout)
{
	auto imageReference = BuildAttachmentReference(attachmentDescriptions["Depth"].index, layout);
	subpassReferences[subpassName].depthReference = imageReference;
}

void Vk::RenderPassBuilder::AttachSubpassDependency(const std::string& subpassName, DependencyStage type, uint32_t subpassIndex, VkPipelineStageFlags stageMask, VkAccessFlags accessMask)
{
	if (type == DependencyStage::SRC)
		subpassReferences[subpassName].dependencySrc = SubpassDependencyData(subpassIndex, stageMask, accessMask);
	else
		subpassReferences[subpassName].dependencyDst = SubpassDependencyData(subpassIndex, stageMask, accessMask);
}

std::shared_ptr<Vk::RenderPass> Vk::RenderPassBuilder::BuildRenderPass()
{
	std::vector<VkSubpassDescription> subpassDescriptions(subpassReferences.size());
	std::vector<VkSubpassDependency> subpassDependencies(subpassReferences.size());
	for (auto& [subpassName, subpassData] : subpassReferences)
	{
		subpassDescriptions[subpassData.index] = BuildSubpassDescription(subpassData.colorRefences, subpassData.depthReference.attachment == VK_IMAGE_LAYOUT_UNDEFINED ? nullptr : &subpassData.depthReference, subpassData.inputRefences);
		subpassDependencies[subpassData.index] = BuildSubpassDependeny(subpassData.dependencySrc, subpassData.dependencyDst);
	}

	std::vector<VkAttachmentDescription> imageDescriptions(attachmentDescriptions.size());
	for (auto& [imageName, imageData] : attachmentDescriptions)
		imageDescriptions[imageData.index] = imageData.description;

	std::shared_ptr<Vk::RenderPass> renderPass = std::make_shared<Vk::RenderPass>(imageDescriptions, subpassDescriptions, subpassDependencies);
	return renderPass;
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

	if(renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(device->Value(), renderPass, nullptr);

	renderPass = VK_NULL_HANDLE;
}