#include "GraphicsPipeline.h"

Vk::GraphicsPipeline::GraphicsPipeline(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyInfo, const VkPipelineDynamicStateCreateInfo& dynamicStateInfo, const VkPipelineViewportStateCreateInfo& viewportStateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationInfo, const VkPipelineMultisampleStateCreateInfo& multisamplingInfo, const VkPipelineColorBlendAttachmentState& colorBlendAttachmentInfo, const VkPipelineColorBlendStateCreateInfo& colorBlendInfo, const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo, const std::span<VkPushConstantRange> pushConstantInfos, const std::span<VkDescriptorSetLayout> descriptorLayoutInfos, const std::span<VkPipelineShaderStageCreateInfo> shaderStageInfos, const VkPipelineRenderingCreateInfo* renderingInfo)
{
	Init(renderPass, subpassIndex, vertexInputInfo, inputAssemblyInfo, dynamicStateInfo,
		viewportStateInfo, rasterizationInfo, multisamplingInfo, colorBlendAttachmentInfo,
		colorBlendInfo, depthStencilInfo, pushConstantInfos, descriptorLayoutInfos, shaderStageInfos, renderingInfo);
}

Vk::GraphicsPipeline::~GraphicsPipeline()
{
	Destroy();
}

const VkPipeline Vk::GraphicsPipeline::Value() const
{
	return pipeline;
}

void Vk::GraphicsPipeline::Init(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyInfo, const VkPipelineDynamicStateCreateInfo& dynamicStateInfo, const VkPipelineViewportStateCreateInfo& viewportStateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationInfo, const VkPipelineMultisampleStateCreateInfo& multisamplingInfo, const VkPipelineColorBlendAttachmentState& colorBlendAttachmentInfo, const VkPipelineColorBlendStateCreateInfo& colorBlendInfo, const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo, const std::span<VkPushConstantRange> pushConstantInfos, const std::span<VkDescriptorSetLayout> descriptorLayoutInfos, const std::span<VkPipelineShaderStageCreateInfo> shaderStageInfos, const VkPipelineRenderingCreateInfo* renderingInfo)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	auto pipelineLayoutInfo = BuildPipelineLayoutInfo(pushConstantInfos, descriptorLayoutInfos);
	VK_CHECK_MESSAGE(vkCreatePipelineLayout(device->Value(), &pipelineLayoutInfo, nullptr, &pipelineLayout), "Failed to create pipeline layout!");

	auto pipelineInfo = BuildGraphicsPipelineInfo(renderPass, subpassIndex, vertexInputInfo, inputAssemblyInfo, dynamicStateInfo,
		viewportStateInfo, rasterizationInfo, multisamplingInfo, colorBlendAttachmentInfo,
		colorBlendInfo, depthStencilInfo, shaderStageInfos, renderingInfo);

	VK_CHECK_MESSAGE(vkCreateGraphicsPipelines(device->Value(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline), "Failed to create graphics pipeline!");
}

void Vk::GraphicsPipeline::Destroy()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if(pipeline != VK_NULL_HANDLE)
		vkDestroyPipeline(device->Value(), pipeline, nullptr);

	if(pipelineLayout != VK_NULL_HANDLE)
		vkDestroyPipelineLayout(device->Value(), pipelineLayout, nullptr);

	pipeline = VK_NULL_HANDLE;
	pipelineLayout = VK_NULL_HANDLE;
}

VkPipelineLayoutCreateInfo Vk::GraphicsPipeline::BuildPipelineLayoutInfo(const std::span<VkPushConstantRange> pushConstantInfos, const std::span<VkDescriptorSetLayout> descriptorLayoutInfos)
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorLayoutInfos.size());
	pipelineLayoutInfo.pSetLayouts = descriptorLayoutInfos.empty() ? nullptr : descriptorLayoutInfos.data();
	pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantInfos.size());
	pipelineLayoutInfo.pPushConstantRanges = pushConstantInfos.empty() ? nullptr : pushConstantInfos.data();

	return pipelineLayoutInfo;
}

VkGraphicsPipelineCreateInfo Vk::GraphicsPipeline::BuildGraphicsPipelineInfo(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyInfo, const VkPipelineDynamicStateCreateInfo& dynamicStateInfo, const VkPipelineViewportStateCreateInfo& viewportStateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationInfo, const VkPipelineMultisampleStateCreateInfo& multisamplingInfo, const VkPipelineColorBlendAttachmentState& colorBlendAttachmentInfo, const VkPipelineColorBlendStateCreateInfo& colorBlendInfo, const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo, const std::span<VkPipelineShaderStageCreateInfo> shaderStageInfos, const VkPipelineRenderingCreateInfo* renderingInfo)
{
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(shaderStageInfos.size());
	pipelineInfo.pStages = shaderStageInfos.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterizationInfo;
	pipelineInfo.pMultisampleState = &multisamplingInfo;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (renderPass)
	{
		pipelineInfo.renderPass = renderPass->Value();
		pipelineInfo.subpass = subpassIndex;
	}

	if (renderingInfo)
	{
		pipelineInfo.pNext = renderingInfo;
	}

	return pipelineInfo;
}

void Vk::GraphicsPipelineBuilder::SetDefaultInfos()
{
	SetDefaultVertexInputInfo();
	SetDefaultInputAssemblyInfo();
	SetDefaultDynamicStateInfo();
	SetDefaultViewPortStateInfo();
	SetDefaultRasterizationInfo();
	SetDefulatMultisamplingInfo();
	SetDefaultColorBlendInfo();
	SetDefaultDepthStencilInfo();
	SetDefaultRenderingInfo();

	pushConstantInfos.clear();
	descriptorLayoutInfos.clear();
	shaderStageInfos.clear();
}

void Vk::GraphicsPipelineBuilder::SetShaderStage(std::shared_ptr<ShaderModule> shaderModule)
{
	shaderStageInfos.push_back(shaderModule->GetPipelineShaderStageInfo());
}

void Vk::GraphicsPipelineBuilder::SetDefaultVertexInputInfo()
{
	vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
}

void Vk::GraphicsPipelineBuilder::SetVertexInputInfo(std::span<VkVertexInputBindingDescription> bindingDescriptions, std::span<VkVertexInputAttributeDescription> attributeDescriptions)
{
	vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.empty() ? nullptr : bindingDescriptions.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.empty() ? nullptr : attributeDescriptions.data();
}

void Vk::GraphicsPipelineBuilder::SetDefaultInputAssemblyInfo()
{
	inputAssemblyInfo = {};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
}

void Vk::GraphicsPipelineBuilder::SetInputAssemblyTopology(VkPrimitiveTopology topology)
{
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

void Vk::GraphicsPipelineBuilder::SetDefaultDynamicStateInfo()
{
	dynamicStateInfo = {};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
}

void Vk::GraphicsPipelineBuilder::SetDynamicStates(std::span<VkDynamicState> dynamicStates)
{
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = dynamicStates.data();
}

void Vk::GraphicsPipelineBuilder::DefaultDynamicStates()
{
	static std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	SetDynamicStates(dynamicStates);
}

void Vk::GraphicsPipelineBuilder::SetDefaultRasterizationInfo()
{
	rasterizationInfo = {};
	rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationInfo.depthClampEnable = VK_FALSE;
	rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo.depthBiasEnable = VK_FALSE;
	rasterizationInfo.depthBiasConstantFactor = 0.0f;
	rasterizationInfo.depthBiasClamp = 0.0f;
	rasterizationInfo.depthBiasSlopeFactor = 0.0f;
}

void Vk::GraphicsPipelineBuilder::SetRasterizationOptions(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth)
{
	rasterizationInfo.polygonMode = polygonMode;
	rasterizationInfo.cullMode = cullMode;
	rasterizationInfo.frontFace = frontFace;
	rasterizationInfo.lineWidth = lineWidth;
}

void Vk::GraphicsPipelineBuilder::DefaultRasterizationInfo()
{
	SetRasterizationOptions(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE, 1.f);
}

void Vk::GraphicsPipelineBuilder::SetPushConstantRange(uint32_t offset, uint32_t size, VkShaderStageFlags shaderStages)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.offset = 0;
	pushConstantRange.size = size;
	pushConstantRange.stageFlags = shaderStages;

	pushConstantInfos.push_back(pushConstantRange);
}

void Vk::GraphicsPipelineBuilder::SetDescriptorSetLayout(const VkDescriptorSetLayout& layout)
{
	descriptorLayoutInfos.push_back(layout);
}

void Vk::GraphicsPipelineBuilder::SetColorAttachmentFormats(std::span<VkFormat> imageFormats)
{
	renderingInfo.colorAttachmentCount = static_cast<uint32_t>(imageFormats.size());
	renderingInfo.pColorAttachmentFormats = imageFormats.data();
}

void Vk::GraphicsPipelineBuilder::SetDepthAttachmentFormat(VkFormat depthFormat)
{
	renderingInfo.depthAttachmentFormat = depthFormat;
}

void Vk::GraphicsPipelineBuilder::SetDefaultViewPortStateInfo()
{
	viewportStateInfo = {};
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.scissorCount = 1;
}

void Vk::GraphicsPipelineBuilder::SetDefulatMultisamplingInfo()
{
	multisamplingInfo = {};
	multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingInfo.sampleShadingEnable = VK_FALSE;
	multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplingInfo.minSampleShading = 1.0f;
	multisamplingInfo.pSampleMask = nullptr;
	multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
	multisamplingInfo.alphaToOneEnable = VK_FALSE;
}

void Vk::GraphicsPipelineBuilder::SetColorBlendOptions(VkBool32 enable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp AlphaBlendOp)
{
	colorBlendAttachmentInfo.blendEnable = enable;
	colorBlendAttachmentInfo.srcColorBlendFactor = srcColorBlendFactor;
	colorBlendAttachmentInfo.dstColorBlendFactor = dstColorBlendFactor;
	colorBlendAttachmentInfo.colorBlendOp = colorBlendOp;
	colorBlendAttachmentInfo.srcAlphaBlendFactor = srcAlphaBlendFactor;
	colorBlendAttachmentInfo.dstAlphaBlendFactor = dstAlphaBlendFactor;
	colorBlendAttachmentInfo.alphaBlendOp = AlphaBlendOp;
}

void Vk::GraphicsPipelineBuilder::SetDefaultColorBlendInfo()
{
	colorBlendAttachmentInfo = {};
	colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	colorBlendInfo = {};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = &colorBlendAttachmentInfo;
	colorBlendInfo.blendConstants[0] = 0.0f;
	colorBlendInfo.blendConstants[1] = 0.0f;
	colorBlendInfo.blendConstants[2] = 0.0f;
	colorBlendInfo.blendConstants[3] = 0.0f;
}

void Vk::GraphicsPipelineBuilder::SetDefaultDepthStencilInfo()
{
	depthStencilInfo = {};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.minDepthBounds = 0.0f;
	depthStencilInfo.maxDepthBounds = 1.0f;
	depthStencilInfo.stencilTestEnable = VK_FALSE;
	depthStencilInfo.front = {};
	depthStencilInfo.back = {};
}

void Vk::GraphicsPipelineBuilder::SetDefaultRenderingInfo()
{
	renderingInfo = {};
	renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
}

void Vk::GraphicsPipelineBuilder::SetDepthStencilOptions(VkBool32 enableTest, VkBool32 enableWrite, VkCompareOp compare)
{
	depthStencilInfo.depthTestEnable = enableTest;
	depthStencilInfo.depthWriteEnable = enableWrite;
	depthStencilInfo.depthCompareOp = compare;
}

void Vk::GraphicsPipelineBuilder::DefaultColorBlendOptions()
{
	SetColorBlendOptions(VK_FALSE, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD);
}

std::shared_ptr<Vk::GraphicsPipeline> Vk::GraphicsPipelineBuilder::BuildPipeline(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex)
{
	std::shared_ptr<Vk::GraphicsPipeline> graphicsPipeline = std::make_shared<Vk::GraphicsPipeline>(
		renderPass, subpassIndex, vertexInputInfo, inputAssemblyInfo, dynamicStateInfo,
		viewportStateInfo, rasterizationInfo, multisamplingInfo, colorBlendAttachmentInfo,
		colorBlendInfo, depthStencilInfo, pushConstantInfos, descriptorLayoutInfos, shaderStageInfos, nullptr);

	return graphicsPipeline;
}

std::shared_ptr<Vk::GraphicsPipeline> Vk::GraphicsPipelineBuilder::BuildDynamicPipeline()
{
	std::shared_ptr<Vk::GraphicsPipeline> graphicsPipeline = std::make_shared<Vk::GraphicsPipeline>(
		nullptr, UINT32_MAX, vertexInputInfo, inputAssemblyInfo, dynamicStateInfo,
		viewportStateInfo, rasterizationInfo, multisamplingInfo, colorBlendAttachmentInfo,
		colorBlendInfo, depthStencilInfo, pushConstantInfos, descriptorLayoutInfos, shaderStageInfos, &renderingInfo);

	return graphicsPipeline;
}
