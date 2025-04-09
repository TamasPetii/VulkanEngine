#include "VulkanManager.h"
#include "Engine/Render/GpuStructs.h"

VulkanManager::VulkanManager()
{
	Initialize();
}

VulkanManager::~VulkanManager()
{
	Cleanup();
}

void VulkanManager::Initialize()
{
	InitSamplers();
	InitShaderModuls();
	InitRenderPasses();
	InitFrameBuffers();
	InitDescriptors();
	InitGraphicsPipelines();
	InitFences();
	InitSemaphores();
}

void VulkanManager::Cleanup()
{
	descriptorPools.clear();
	samplers.clear();
	renderPasses.clear();
	shaderModuls.clear();
	graphicsPipelines.clear();
	frameDependentBuffers.clear();
	frameDependentFrameBuffers.clear();
	frameDependentDescriptorSets.clear();
}

void VulkanManager::RegisterSampler(const std::string& name, const Vk::ImageSamplerConfig& config)
{
	RegisterSampler(name, std::make_shared<Vk::ImageSampler>(config));
}

void VulkanManager::RegisterSampler(const std::string& name, std::shared_ptr<Vk::ImageSampler> sampler)
{
	samplers[name] = sampler;
}

std::shared_ptr<Vk::ImageSampler> VulkanManager::GetSampler(const std::string& name) const
{
	if (samplers.find(name) == samplers.end())
		return nullptr;

	return samplers.at(name);
}

void VulkanManager::RegisterRenderPass(const std::string& name, std::shared_ptr<Vk::RenderPass> renderPass)
{
	renderPasses[name] = renderPass;
}

std::shared_ptr<Vk::RenderPass> VulkanManager::GetRenderPass(const std::string& name) const
{
	if (renderPasses.find(name) == renderPasses.end())
		return nullptr;

	return renderPasses.at(name);
}

void VulkanManager::RegisterShaderModule(const std::string& name, std::shared_ptr<Vk::ShaderModule> shaderModule)
{
	shaderModuls[name] = shaderModule;
}

std::shared_ptr<Vk::ShaderModule> VulkanManager::GetShaderModule(const std::string& name) const
{
	if (shaderModuls.find(name) == shaderModuls.end())
		return nullptr;

	return shaderModuls.at(name);
}

void VulkanManager::RegisterGraphicsPipeline(const std::string& name, std::shared_ptr<Vk::GraphicsPipeline> graphicsPipeline)
{
	graphicsPipelines[name] = graphicsPipeline;
}

std::shared_ptr<Vk::GraphicsPipeline> VulkanManager::GetGraphicsPipeline(const std::string& name) const
{
	if (graphicsPipelines.find(name) == graphicsPipelines.end())
		return nullptr;

	return graphicsPipelines.at(name);
}

void VulkanManager::MarkFrameBufferToResize(const std::string& name, uint32_t index, uint32_t width, uint32_t height)
{
	frameBuffersToResize.insert(std::make_tuple(name, index, width, height));
}

void VulkanManager::ResizeMarkedFrameBuffers(uint32_t frameIndex)
{
	for (auto it = frameBuffersToResize.begin(); it != frameBuffersToResize.end();)
	{
		auto& [name, index, width, height] = *it;

		if (index == frameIndex)
		{
			//Resize Framebuffer
			GetFrameDependentFrameBuffer(name, index)->Resize(width, height);

			//Regenerate descriptor for framebuffers, need some association mechanism between fbo and dsc
			if (name == "Main")
			{
				GetFrameDependentDescriptorSet("MainFrameBuffer", index)->Free(GetDescriptorPool("Main")->Value());
				InitMainFramebufferDescriptorSet(index);
			}

			//std::cout << std::format("Resized framebuffer {} index {}: {} {}", name, index, width, height) << std::endl;

			it = frameBuffersToResize.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void VulkanManager::RegisterFrameDependentFrameBuffer(const std::string& name, std::shared_ptr<Vk::FrameBuffer> frameBuffer, uint32_t frameIndex)
{
	if (frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return;

	frameDependentFrameBuffers[name][frameIndex] = frameBuffer;
}

std::shared_ptr<Vk::FrameBuffer> VulkanManager::GetFrameDependentFrameBuffer(const std::string& name, uint32_t frameIndex) const
{
	if (frameDependentFrameBuffers.find(name) == frameDependentFrameBuffers.end() || frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return nullptr;

	return frameDependentFrameBuffers.at(name)[frameIndex];
}

void VulkanManager::RegisterDescriptorPool(const std::string& name, std::shared_ptr<Vk::DescriptorPool> pool)
{
	descriptorPools[name] = pool;
}

std::shared_ptr<Vk::DescriptorPool> VulkanManager::GetDescriptorPool(const std::string& name) const
{
	if (descriptorPools.find(name) == descriptorPools.end())
		return nullptr;

	return descriptorPools.at(name);
}

void VulkanManager::RegisterFrameDependentDescriptorSet(const std::string& name, std::shared_ptr<Vk::DescriptorSet> set, uint32_t frameIndex)
{
	if (frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return;

	frameDependentDescriptorSets[name][frameIndex] = set;
}

std::shared_ptr<Vk::DescriptorSet> VulkanManager::GetFrameDependentDescriptorSet(const std::string& name, uint32_t frameIndex) const
{
	if (frameDependentDescriptorSets.find(name) == frameDependentDescriptorSets.end() || frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return nullptr;

	return frameDependentDescriptorSets.at(name)[frameIndex];
}

void VulkanManager::RegisterFrameDependentFence(const std::string& name, std::shared_ptr<Vk::Fence> fence, uint32_t frameIndex)
{
	if (frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return;

	frameDependentFences[name][frameIndex] = fence;
}

std::shared_ptr<Vk::Fence> VulkanManager::GetFrameDependentFence(const std::string& name, uint32_t frameIndex)
{
	if (frameDependentFences.find(name) == frameDependentFences.end() || frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return nullptr;

	return frameDependentFences.at(name)[frameIndex];
}

void VulkanManager::RegisterFrameDependentSemaphore(const std::string& name, std::shared_ptr<Vk::Semaphore> semaphore, uint32_t frameIndex)
{
	if (frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return;

	frameDependentSemaphores[name][frameIndex] = semaphore;
}

std::shared_ptr<Vk::Semaphore> VulkanManager::GetFrameDependentSemaphore(const std::string& name, uint32_t frameIndex)
{
	if (frameDependentSemaphores.find(name) == frameDependentSemaphores.end() || frameIndex >= MAX_FRAMES_IN_FLIGHTS)
		return nullptr;

	return frameDependentSemaphores.at(name)[frameIndex];
}

void VulkanManager::InitSamplers()
{
	{
		Vk::ImageSamplerConfig config{};
		config.magFilter = VK_FILTER_LINEAR;
		config.minFilter = VK_FILTER_LINEAR;
		config.addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		config.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		config.mipMapFilter = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		config.anisotropyEnable = false;

		RegisterSampler("Linear", config);
	}

	{
		Vk::ImageSamplerConfig config{};
		config.magFilter = VK_FILTER_NEAREST;
		config.minFilter = VK_FILTER_NEAREST;
		config.addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		config.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		config.mipMapFilter = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		config.anisotropyEnable = false;

		RegisterSampler("Nearest", config);
	}
}

void VulkanManager::InitRenderPasses()
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

void VulkanManager::InitFrameBuffers()
{
	Vk::ImageSpecification mainImageSpec;
	mainImageSpec.type = VK_IMAGE_TYPE_2D;
	mainImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	mainImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	mainImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	mainImageSpec.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	mainImageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	mainImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::ImageSpecification colorImageSpec;
	colorImageSpec.type = VK_IMAGE_TYPE_2D;
	colorImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	colorImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	colorImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	colorImageSpec.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	colorImageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	colorImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::ImageSpecification normalImageSpec;
	normalImageSpec.type = VK_IMAGE_TYPE_2D;
	normalImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	normalImageSpec.format = VK_FORMAT_R16G16B16A16_SFLOAT;
	normalImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	normalImageSpec.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	normalImageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	normalImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::ImageSpecification entityImageSpec;
	entityImageSpec.type = VK_IMAGE_TYPE_2D;
	entityImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	entityImageSpec.format = VK_FORMAT_R32_UINT;
	entityImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	entityImageSpec.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	entityImageSpec.aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	entityImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::ImageSpecification depthImageSpec;
	depthImageSpec.type = VK_IMAGE_TYPE_2D;
	depthImageSpec.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthImageSpec.format = VK_FORMAT_D32_SFLOAT;
	depthImageSpec.tiling = VK_IMAGE_TILING_OPTIMAL;
	depthImageSpec.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depthImageSpec.aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthImageSpec.memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	Vk::FrameBufferBuilder frameBufferBuilder;
	frameBufferBuilder.SetSize(1, 1)
		.AddImageSpecification("Main", 0, mainImageSpec)
		.AddImageSpecification("Color", 1, colorImageSpec)
		.AddImageSpecification("Normal", 2, normalImageSpec)
		.AddImageSpecification("Entity", 3, entityImageSpec)
		.AddDepthSpecification(4, depthImageSpec);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; ++i)
		RegisterFrameDependentFrameBuffer("Main", frameBufferBuilder.BuildDynamic(), i);
}

void VulkanManager::InitDescriptors()
{
	Vk::DescriptorPoolBuilder poolBuilder;
	poolBuilder
		.SetSetSize(MAX_FRAMES_IN_FLIGHTS)
		.SetPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 10)
		.SetPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10)
		.SetPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 10);

	RegisterDescriptorPool("Main", poolBuilder.BuildDescriptorPool());

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; ++i)
		InitMainFramebufferDescriptorSet(i);
}

void VulkanManager::InitShaderModuls()
{
	//Basic Shaders
	RegisterShaderModule("BasicVert", std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.vert", VK_SHADER_STAGE_VERTEX_BIT));
	RegisterShaderModule("BasicFrag", std::make_shared<Vk::ShaderModule>("../Engine/Shaders/Basic.frag", VK_SHADER_STAGE_FRAGMENT_BIT));

	//Geometry Shaders (Deferred Pre)
	RegisterShaderModule("DeferredPreVert", std::make_shared<Vk::ShaderModule>("../Engine/Shaders/DeferredPre.vert", VK_SHADER_STAGE_VERTEX_BIT));
	RegisterShaderModule("DeferredPreFrag", std::make_shared<Vk::ShaderModule>("../Engine/Shaders/DeferredPre.frag", VK_SHADER_STAGE_FRAGMENT_BIT));

	//Deferred Direction Light Shaders
	RegisterShaderModule("DeferredDirVert", std::make_shared<Vk::ShaderModule>("../Engine/Shaders/DeferredDir.vert", VK_SHADER_STAGE_VERTEX_BIT));
	RegisterShaderModule("DeferredDirFrag", std::make_shared<Vk::ShaderModule>("../Engine/Shaders/DeferredDir.frag", VK_SHADER_STAGE_FRAGMENT_BIT));
}

void VulkanManager::InitGraphicsPipelines()
{
	{
		uint32_t pushConsantSize = sizeof(VkDeviceAddress);

		Vk::GraphicsPipelineBuilder pipelineBuilder;
		pipelineBuilder
			.ResetToDefault()
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
			.AddPushConstant(0, pushConsantSize, VK_SHADER_STAGE_VERTEX_BIT);

		RegisterGraphicsPipeline("Basic", pipelineBuilder.BuildDynamic());
	}

	{
		uint32_t pushConsantSize = sizeof(GpuPushConstant);
			 
		Vk::GraphicsPipelineBuilder pipelineBuilder;
		pipelineBuilder
			.ResetToDefault()
			.AddShaderStage(shaderModuls["DeferredPreVert"])
			.AddShaderStage(shaderModuls["DeferredPreFrag"])
			.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
			.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR)
			.SetVertexInput({}, {})
			.SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
			.SetRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE)
			.SetMultisampling(VK_SAMPLE_COUNT_1_BIT)
			.SetDepthStencil(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS)
			.SetColorBlend(VK_FALSE)
			.AddColorBlendAttachment(VK_FALSE)
			.AddColorBlendAttachment(VK_FALSE)
			.AddColorBlendAttachment(VK_FALSE)
			.SetColorAttachmentFormats(VK_FORMAT_R16G16B16A16_SFLOAT, 0)
			.SetColorAttachmentFormats(VK_FORMAT_R16G16B16A16_SFLOAT, 1)
			.SetColorAttachmentFormats(VK_FORMAT_R32_UINT, 2)
			.SetDepthAttachmentFormat(VK_FORMAT_D32_SFLOAT)
			.AddPushConstant(0, pushConsantSize, VK_SHADER_STAGE_VERTEX_BIT);

		RegisterGraphicsPipeline("DeferredPre", pipelineBuilder.BuildDynamic());
	}

	{
		Vk::GraphicsPipelineBuilder pipelineBuilder;
		pipelineBuilder
			.ResetToDefault()
			.AddShaderStage(shaderModuls["DeferredDirVert"])
			.AddShaderStage(shaderModuls["DeferredDirFrag"])
			.AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT)
			.AddDynamicState(VK_DYNAMIC_STATE_SCISSOR)
			.SetVertexInput({}, {})
			.SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)
			.SetRasterization(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE)
			.SetMultisampling(VK_SAMPLE_COUNT_1_BIT)
			.SetDepthStencil(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS)
			.SetColorBlend(VK_FALSE)
			.AddColorBlendAttachment(VK_FALSE)
			.SetColorAttachmentFormats(VK_FORMAT_R16G16B16A16_SFLOAT, 0)
			.AddDescriptorSetLayout(GetFrameDependentDescriptorSet("MainFrameBuffer", 0)->Layout());

		RegisterGraphicsPipeline("DeferredDir", pipelineBuilder.BuildDynamic());		
	}
}

void VulkanManager::InitFences()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; ++i)
		RegisterFrameDependentFence("InFlight", std::make_shared<Vk::Fence>(true), i);
}

void VulkanManager::InitSemaphores()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHTS; ++i)
	{
		RegisterFrameDependentSemaphore("ImageAvailable", std::make_shared<Vk::Semaphore>(), i);
		RegisterFrameDependentSemaphore("RenderFinished", std::make_shared<Vk::Semaphore>(), i);
	}
}

void VulkanManager::InitMainFramebufferDescriptorSet(uint32_t frameIndex)
{
	Vk::DescriptorSetBuilder setBuilder;
	setBuilder
		.AddDescriptorLayoutImage(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, GetFrameDependentFrameBuffer("Main", frameIndex)->GetImage("Main")->GetImageView(), GetSampler("Nearest")->Value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		.AddDescriptorLayoutImage(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, GetFrameDependentFrameBuffer("Main", frameIndex)->GetImage("Color")->GetImageView(), GetSampler("Nearest")->Value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		.AddDescriptorLayoutImage(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, GetFrameDependentFrameBuffer("Main", frameIndex)->GetImage("Normal")->GetImageView(), GetSampler("Nearest")->Value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		.AddDescriptorLayoutImage(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, GetFrameDependentFrameBuffer("Main", frameIndex)->GetImage("Entity")->GetImageView(), GetSampler("Nearest")->Value(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	RegisterFrameDependentDescriptorSet("MainFrameBuffer", setBuilder.BuildDescriptorSet(GetDescriptorPool("Main")->Value()), frameIndex);
}
