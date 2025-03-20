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

void RenderContext::SetViewPortSize(uint32_t width, uint32_t height)
{
	if (viewPortWidth != width || viewPortHeight != height)
	{
		viewPortResize = true;
		viewPortWidth = width;
		viewPortHeight = height;
	}
}

void RenderContext::ResizeViewportResources()
{
	auto device = Vk::VulkanContext::GetContext()->GetDevice();
	auto renderContext = RenderContext::GetContext();
	vkDeviceWaitIdle(device->Value());

	for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
	{
		renderContext->GetFrameBuffer("main", i)->Resize(viewPortWidth, viewPortHeight);
		
		Vk::DescriptorSetBuilder setBuilder;
		descriptorSets[i]->Free(descriptorPool->Value());
		descriptorSets[i] = setBuilder
			.AddDescriptorLayoutImage(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, GetFrameBuffer("main", i)->GetImage("main")->GetImageView(), GetSampler("nearest")->Value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			.BuildDescriptorSet(descriptorPool->Value());
	}

	//std::cout << std::format("Resized framebuffers {} {}", viewPortWidth, viewPortHeight) << std::endl;
}

bool RenderContext::ShouldViewportResize()
{
	return viewPortResize;
}

void RenderContext::ResetViewportResize()
{
	viewPortResize = false;
}

std::pair<uint32_t, uint32_t> RenderContext::GetViewportSize()
{
	return std::pair<uint32_t, uint32_t>(viewPortWidth, viewPortHeight);
}

uint32_t RenderContext::GetFramesInFlightIndex()
{
	return framesInFlightIndex;
}

std::shared_ptr<Vk::ImageSampler> RenderContext::GetSampler(const std::string& name)
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
	InitDescriptors();

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
	samplers.clear();
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
		.AddPushConstant(0, sizeof(VkDeviceAddress), VK_SHADER_STAGE_VERTEX_BIT)
		.BuildDynamic();
}

void RenderContext::InitSamplers()
{
	{
		Vk::ImageSamplerConfig config{};
		config.magFilter = VK_FILTER_LINEAR;
		config.minFilter = VK_FILTER_LINEAR;
		config.addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		config.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		config.mipMapFilter = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		config.anisotropyEnable = true;
		
		samplers["linear"] = std::make_shared<Vk::ImageSampler>(config);
	}

	{
		Vk::ImageSamplerConfig config{};
		config.magFilter = VK_FILTER_NEAREST;
		config.minFilter = VK_FILTER_NEAREST;
		config.addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		config.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		config.mipMapFilter = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		config.anisotropyEnable = true;

		samplers["nearest"] = std::make_shared<Vk::ImageSampler>(config);
	}
}

void RenderContext::UpdateFramesInFlightIndex()
{
	framesInFlightIndex = (framesInFlightIndex + 1) % FRAMES_IN_FLIGHT;
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

void RenderContext::InitDescriptors()
{
	descriptorSets.resize(FRAMES_IN_FLIGHT);

	Vk::DescriptorPoolBuilder poolBuilder;
	descriptorPool = poolBuilder
		.SetSetSize(10)
		.SetPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10)
		.SetPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10)
		.BuildDescriptorPool();

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		Vk::DescriptorSetBuilder setBuilder;
		descriptorSets[i] = setBuilder
			.AddDescriptorLayoutImage(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, GetFrameBuffer("main", i)->GetImage("main")->GetImageView(), GetSampler("nearest")->Value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			.BuildDescriptorSet(descriptorPool->Value());
	}
}