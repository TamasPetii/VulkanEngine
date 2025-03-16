#include "FrameBuffer.h"

Vk::FrameBuffer::FrameBuffer(FrameBufferConfig& config) :
	config(config)
{
	Initialize();
}

Vk::FrameBuffer::~FrameBuffer()
{
	Cleanup();
}

void Vk::FrameBuffer::Resize(uint32_t width, uint32_t height)
{
	config.width = width;
	config.height = height;
	Cleanup();
	Initialize();
}

VkFramebuffer Vk::FrameBuffer::GetFrameBuffer() const
{
	return frameBuffer;
}

VkExtent2D Vk::FrameBuffer::GetSize() const
{
	return VkExtent2D(config.width, config.height);
}

std::shared_ptr<Vk::Image> Vk::FrameBuffer::GetImage(const std::string& imageName) const
{
	auto& imageData = config.imageSpecifications.at(imageName);
	return images[imageData.first];
}

bool Vk::FrameBuffer::Initialize()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	std::vector<VkImageView> imageViews(config.imageSpecifications.size());
	images.resize(config.imageSpecifications.size());

	for (auto& [imageName, imageData] : config.imageSpecifications)
	{
		imageData.second.width = config.width;
		imageData.second.height = config.height;
		images[imageData.first] = std::make_shared<Image>(imageData.second);
		imageViews[imageData.first] = images[imageData.first]->GetImageView();
	}

	if (config.renderPass.has_value())
	{
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = config.renderPass.value();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(imageViews.size());
		framebufferInfo.pAttachments = imageViews.data();
		framebufferInfo.width = config.width;
		framebufferInfo.height = config.height;
		framebufferInfo.layers = 1;

		VK_CHECK_MESSAGE(vkCreateFramebuffer(device->Value(), &framebufferInfo, nullptr, &frameBuffer), "Failed to create framebuffer!");
	}

	return true;
}

void Vk::FrameBuffer::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	images.clear();

	if (frameBuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(device->Value(), frameBuffer, nullptr);
		frameBuffer = VK_NULL_HANDLE;
	}
}


Vk::FrameBufferBuilder::FrameBufferBuilder()
{
	ResetToDefault();
}

void Vk::FrameBufferBuilder::ResetToDefault()
{
	config = FrameBufferConfig{};
}

Vk::FrameBufferBuilder& Vk::FrameBufferBuilder::SetSize(uint32_t width, uint32_t height)
{
	config.width = width;
	config.height = height;
	return *this;
}

Vk::FrameBufferBuilder& Vk::FrameBufferBuilder::AddImageSpecification(const std::string& imageName,	uint32_t index,	const ImageSpecification& specification)
{
	config.imageSpecifications[imageName] = { index, specification };
	return *this;
}

Vk::FrameBufferBuilder& Vk::FrameBufferBuilder::AddDepthSpecification(uint32_t index, const ImageSpecification& specification)
{
	return AddImageSpecification("depth", index, specification);
}

std::shared_ptr<Vk::FrameBuffer> Vk::FrameBufferBuilder::Build(VkRenderPass renderPass)
{
	config.renderPass = renderPass;
	return std::make_shared<FrameBuffer>(config);
}

std::shared_ptr<Vk::FrameBuffer> Vk::FrameBufferBuilder::BuildDynamic()
{
	return std::make_shared<FrameBuffer>(config);
}