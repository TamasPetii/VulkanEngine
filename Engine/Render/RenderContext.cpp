#include "RenderContext.h"
#include "Vertex.h"

RenderContext* RenderContext::context = nullptr;
bool RenderContext::initialized = false;

RenderContext::~RenderContext()
{
	Destory();
}

RenderContext* RenderContext::GetContext()
{
	if (context == nullptr)
		context = new RenderContext();

	return context;
}

void RenderContext::DestroyContext()
{
	if (context != nullptr)
	{
		delete context;
		context = nullptr;
	}
}

std::shared_ptr<Vk::RenderPass> RenderContext::GetRenderPass(const std::string& name)
{
	if (renderPasses.find(name) == renderPasses.end())
		return nullptr;

	return renderPasses.at(name);
}

std::shared_ptr<Vk::ShaderModule> RenderContext::GetShaderModule(const std::string& name)
{
	if (shaderModuls.find(name) == shaderModuls.end())
		return nullptr;

	return shaderModuls.at(name);
}

std::shared_ptr<Vk::GraphicsPipeline> RenderContext::GetGraphicsPipeline(const std::string& name)
{
	if (graphicsPipelines.find(name) == graphicsPipelines.end())
		return nullptr;

	return graphicsPipelines.at(name);
}

std::shared_ptr<Vk::FrameBuffer> RenderContext::GetFrameBuffer(const std::string& name, uint32_t index)
{
	if (frameBuffers.find(name) == frameBuffers.end())
		return nullptr;

	if(index >= frameBuffers.at(name).size())
		return nullptr;

	return frameBuffers.at(name)[index];
}

const VkSampler& RenderContext::GetSampler(const std::string& name)
{
	if (samplers.find(name) == samplers.end())
		return nullptr;

	return samplers.at(name);
}

void RenderContext::Init()
{
	if (initialized)
		return;

	InitShaderModules();
	InitRenderPasses();
	InitGraphicsPipelines();
	InitFrameBuffers();
	InitSamplers();

	initialized = true;
}

void RenderContext::Destory()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	vkDeviceWaitIdle(device->Value());

	shaderModuls.clear();
	graphicsPipelines.clear();
	frameBuffers.clear();
	renderPasses.clear();

	for (auto& [name, sampler] : samplers)
		vkDestroySampler(device->Value(), sampler, nullptr);
}

void RenderContext::InitShaderModules()
{
	shaderModuls["BasicVert"] = std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.vert", VK_SHADER_STAGE_VERTEX_BIT);
	shaderModuls["BasicFrag"] = std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.frag", VK_SHADER_STAGE_FRAGMENT_BIT);
}

void RenderContext::InitRenderPasses()
{
	/*
	Vk::RenderPassBuilder renderPassBuilder;
	renderPassBuilder.RegisterSubpass("mainSubpass", 0);
	renderPassBuilder.AttachImageDescription("main", 0, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	renderPassBuilder.AttachImageReferenceToSubpass("mainSubpass", "main", VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachDepthDescription(1, VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachDepthReferenceToSubpass("mainSubpass", VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	renderPassBuilder.AttachSubpassDependency("mainSubpass", Vk::DependencyStage::SRC, VK_SUBPASS_EXTERNAL, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_NONE);
	renderPassBuilder.AttachSubpassDependency("mainSubpass", Vk::DependencyStage::DST, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
	renderPass = renderPassBuilder.BuildRenderPass();
	*/
}

void RenderContext::InitGraphicsPipelines()
{
	std::vector<VkVertexInputBindingDescription> vertexBindingDescirption = { Vertex::GetBindingDescription() };
	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescription = Vertex::GetAttributeDescriptions();

	Vk::GraphicsPipelineBuilder pipelineBuilder;

	graphicsPipelines["main"] = pipelineBuilder
		.AddShaderStage(shaderModuls["BasicVert"])
		.AddShaderStage(shaderModuls["BasicFrag"])
		.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
		.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR)
		.SetVertexInput({}, {})
		.SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.SetRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE)
		.SetMultisampling(VK_SAMPLE_COUNT_1_BIT)
		.SetDepthStencil(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS)
		.SetColorBlend(VK_FALSE)
		.AddColorBlendAttachment(VK_FALSE)
		.SetColorAttachmentFormats(VK_FORMAT_R16G16B16A16_SFLOAT, 0)
		.SetDepthAttachmentFormat(VK_FORMAT_D32_SFLOAT)
		.BuildDynamic();
}

void RenderContext::InitSamplers()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto physicalDevice = Vk::VulkanContext::GetContext()->GetPhysicalDevice();

	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice->Value(), &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		VK_CHECK_MESSAGE(vkCreateSampler(device->Value(), &samplerInfo, nullptr, &samplers["linear"]), "Failed to create texture sampler!");
	}

	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice->Value(), &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		VK_CHECK_MESSAGE(vkCreateSampler(device->Value(), &samplerInfo, nullptr, &samplers["nearest"]), "Failed to create texture sampler!");
	}
}

void RenderContext::InitFrameBuffers()
{
	auto swapChainExtent = Vk::VulkanContext::GetContext()->GetSwapChain()->GetExtent();

	Vk::ImageSpecification mainImageSpec;
	mainImageSpec.type = VK_IMAGE_TYPE_2D;
	mainImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	mainImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	mainImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	mainImageSpec.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	mainImageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	mainImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::ImageSpecification depthImageSpec;
	depthImageSpec.type = VK_IMAGE_TYPE_2D;
	depthImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageSpec.format = VK_FORMAT_D32_SFLOAT;
	depthImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageSpec.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthImageSpec.aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::FrameBufferBuilder frameBufferBuilder;
	frameBufferBuilder.SetSize(swapChainExtent.width, swapChainExtent.height)
		.AddImageSpecification("main", 0, mainImageSpec)
		.AddDepthSpecification(1, depthImageSpec);

	frameBuffers["main"].resize(FRAMES_IN_FLIGHT);
	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
		frameBuffers["main"][i] = frameBufferBuilder.BuildDynamic();
}