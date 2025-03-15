#include "FrameBuffer.h"

Vk::FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, std::shared_ptr<RenderPass> renderPass) :
	width(width),
	height(height),
	renderPass(renderPass)
{
}

Vk::FrameBuffer::~FrameBuffer()
{
	Destroy();
}

const VkFramebuffer Vk::FrameBuffer::Value() const
{
	return frameBuffer;
}

const std::shared_ptr<Vk::Image> Vk::FrameBuffer::GetImage(const std::string& imageName) const
{
	if (images.find(imageName) == images.end())
		return nullptr;

	return images.at(imageName).image;
}

void Vk::FrameBuffer::AttachImage(const std::string& imageName, uint32_t index, const ImageSpecification& specification)
{
	images[imageName].index = index;
	images[imageName].specification = specification;
}

void Vk::FrameBuffer::Init()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	std::vector<VkImageView> imageViewAttachments(images.size());
	
	for (auto& [imageName, imageData] : images)
	{
		imageData.specification.width = width;
		imageData.specification.height = height;
		imageData.image = std::make_shared<Image>(imageData.specification);
		imageViewAttachments[imageData.index] = imageData.image->Value();
	}

	auto framebufferInfo = BuildFramebufferInfo(imageViewAttachments);
	VK_CHECK_MESSAGE(vkCreateFramebuffer(device->Value(), &framebufferInfo, nullptr, &frameBuffer), "Failed to create framebuffer!");
}

void Vk::FrameBuffer::Resize(uint32_t width, uint32_t height)
{
	if (this->width == width && this->height == height)
		return;

	this->width = width;
	this->height = height;
	Destroy();
	Init();
}

void Vk::FrameBuffer::Destroy()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	for (auto& [imageName, imageData] : images)
		imageData.image->Destroy();

	if(frameBuffer != VK_NULL_HANDLE)
		vkDestroyFramebuffer(device->Value(), frameBuffer, nullptr);

	frameBuffer = VK_NULL_HANDLE;
}

VkFramebufferCreateInfo Vk::FrameBuffer::BuildFramebufferInfo(std::span<VkImageView> imageViewAttachments)
{
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = renderPass->Value();
	framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViewAttachments.size());
	framebufferInfo.pAttachments = imageViewAttachments.data();
	framebufferInfo.width = width;
	framebufferInfo.height = height;
	framebufferInfo.layers = 1;

	return framebufferInfo;
}

void Vk::FrameBufferBuilder::Reset()
{
	width = 0;
	height = 0;
	imageSpecifications.clear();
}

void Vk::FrameBufferBuilder::SetInitialSize(uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;
}

void Vk::FrameBufferBuilder::AttachImageSpec(const std::string& imageName, uint32_t index, const ImageSpecification& specification)
{
	imageSpecifications[imageName].index = index;
	imageSpecifications[imageName].specification = specification;
}

void Vk::FrameBufferBuilder::AttachDepthSpec(uint32_t index, const ImageSpecification& specification)
{
	AttachImageSpec("depth", index, specification);
}

std::shared_ptr<Vk::FrameBuffer> Vk::FrameBufferBuilder::BuildFrameBuffer(std::shared_ptr<RenderPass> renderPass)
{
	std::shared_ptr<FrameBuffer> frameBuffer = std::make_shared<FrameBuffer>(width, height, renderPass);

	for (auto& [imageName, imageData] : imageSpecifications)
		frameBuffer->AttachImage(imageName, imageData.index, imageData.specification);

	frameBuffer->Init();

	return frameBuffer;
}
