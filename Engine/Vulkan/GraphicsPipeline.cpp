#include "GraphicsPipeline.h"

Vk::GraphicsPipeline::GraphicsPipeline(GraphicsPipelineConfig& config)
{
	Initialize(config);
}

Vk::GraphicsPipeline::~GraphicsPipeline()
{
	Cleanup();
}

bool Vk::GraphicsPipeline::Initialize(GraphicsPipelineConfig& config)
{
	auto device = VulkanContext::GetContext()->GetDevice();

	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = static_cast<uint32_t>(config.descriptorLayouts.size());
	layoutInfo.pSetLayouts = config.descriptorLayouts.data();
	layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(config.pushConstants.size());
	layoutInfo.pPushConstantRanges = config.pushConstants.data();

	VK_CHECK_MESSAGE(vkCreatePipelineLayout(device->Value(), &layoutInfo, nullptr, &pipelineLayout), "Failed to create pipeline layout");

	//DynamicStateInfo configuration
	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStates.size());
	dynamicStateInfo.pDynamicStates = config.dynamicStates.data();

	//ColorBlendInfo and Attachments configuration
	config.colorBlendInfo.attachmentCount = static_cast<uint32_t>(config.colorBlendAttachments.size());
	config.colorBlendInfo.pAttachments = config.colorBlendAttachments.data();

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(config.shaderStages.size());
	pipelineInfo.pStages = config.shaderStages.data();
	pipelineInfo.pVertexInputState = &config.vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
	pipelineInfo.pViewportState = &config.viewportStateInfo;
	pipelineInfo.pRasterizationState = &config.rasterizationInfo;
	pipelineInfo.pMultisampleState = &config.multisampleInfo;
	pipelineInfo.pColorBlendState = &config.colorBlendInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = pipelineLayout;

	if (config.depthStencilInfo.has_value())
	{
		pipelineInfo.pDepthStencilState = &config.depthStencilInfo.value();
	}

	if (config.renderPass.has_value())
	{
		pipelineInfo.renderPass = config.renderPass.value();
		pipelineInfo.subpass = config.subpassIndex.value();
	}

	VkPipelineRenderingCreateInfo dynamicRenderingInfo{};
	if (config.dynamicImageFormats.has_value())
	{
		dynamicRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		dynamicRenderingInfo.colorAttachmentCount = static_cast<uint32_t>(config.dynamicImageFormats.value().first.size());
		dynamicRenderingInfo.pColorAttachmentFormats = config.dynamicImageFormats.value().first.data();
		dynamicRenderingInfo.depthAttachmentFormat = config.dynamicImageFormats.value().second;

		pipelineInfo.pNext = &dynamicRenderingInfo;
	}

	VK_CHECK_MESSAGE(vkCreateGraphicsPipelines(device->Value(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline), "Failed to create graphics pipeline");

	return true;
}

void Vk::GraphicsPipeline::Cleanup()
{
	auto device = VulkanContext::GetContext()->GetDevice();

	if (pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(device->Value(), pipeline, nullptr);
		pipeline = VK_NULL_HANDLE;
	}
	if (pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(device->Value(), pipelineLayout, nullptr);
		pipelineLayout = VK_NULL_HANDLE;
	}
}

Vk::GraphicsPipelineBuilder::GraphicsPipelineBuilder()
{
	ResetToDefault();
}

void Vk::GraphicsPipelineBuilder::ResetToDefault()
{
	config = GraphicsPipelineConfig{};
	config.vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	config.viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	config.viewportStateInfo.viewportCount = 1;
	config.viewportStateInfo.scissorCount = 1;
	config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	config.rasterizationInfo.lineWidth = 1.0f;
	config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::AddShaderStage(std::shared_ptr<ShaderModule> shader)
{
	config.shaderStages.push_back(shader->GetPipelineShaderStageInfo());
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetVertexInput(std::span<VkVertexInputBindingDescription> bindings, std::span<VkVertexInputAttributeDescription> attributes)
{
	config.vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
	config.vertexInputInfo.pVertexBindingDescriptions = bindings.data();
	config.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
	config.vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetPrimitiveTopology(VkPrimitiveTopology topology, VkBool32 primitiveRestart)
{
	config.inputAssemblyInfo.topology = topology;
	config.inputAssemblyInfo.primitiveRestartEnable = primitiveRestart;
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetRasterization(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth)
{
	config.rasterizationInfo.polygonMode = polygonMode;
	config.rasterizationInfo.cullMode = cullMode;
	config.rasterizationInfo.frontFace = frontFace;
	config.rasterizationInfo.lineWidth = lineWidth;
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetDepthStencil(VkBool32 depthTest, VkBool32 depthWrite, VkCompareOp depthCompare, VkBool32 stencilTest)
{
	config.depthStencilInfo = VkPipelineDepthStencilStateCreateInfo{};
	config.depthStencilInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	config.depthStencilInfo->depthTestEnable = depthTest;
	config.depthStencilInfo->depthWriteEnable = depthWrite;
	config.depthStencilInfo->depthCompareOp = depthCompare;
	config.depthStencilInfo->stencilTestEnable = stencilTest;
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::AddDynamicState(VkDynamicState state)
{
	config.dynamicStates.push_back(state);
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetMultisampling(VkSampleCountFlagBits samples)
{
	config.multisampleInfo.rasterizationSamples = samples;
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::AddColorBlendAttachment(VkBool32 blendEnable, VkBlendFactor srcColor, VkBlendFactor dstColor, VkBlendOp colorOp, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha, VkBlendOp alphaOp)
{
	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.blendEnable = blendEnable;
	blendAttachment.srcColorBlendFactor = srcColor;
	blendAttachment.dstColorBlendFactor = dstColor;
	blendAttachment.colorBlendOp = colorOp;
	blendAttachment.srcAlphaBlendFactor = srcAlpha;
	blendAttachment.dstAlphaBlendFactor = dstAlpha;
	blendAttachment.alphaBlendOp = alphaOp;
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	config.colorBlendAttachments.push_back(blendAttachment);
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetColorBlend(VkBool32 enable, VkLogicOp operation)
{
	config.colorBlendInfo.logicOpEnable = enable;
	config.colorBlendInfo.logicOp = operation;
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::AddPushConstant(uint32_t offset, uint32_t size, VkShaderStageFlags stages)
{
	VkPushConstantRange pushConstant{};
	pushConstant.offset = offset;
	pushConstant.size = size;
	pushConstant.stageFlags = stages;
	config.pushConstants.push_back(pushConstant);
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::AddDescriptorSetLayout(VkDescriptorSetLayout layout)
{
	config.descriptorLayouts.push_back(layout);
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetColorAttachmentFormats(VkFormat format, uint32_t index)
{
	if (!config.dynamicImageFormats.has_value())
		config.dynamicImageFormats = std::pair<std::vector<VkFormat>, VkFormat>();

	if (index >= config.dynamicImageFormats->first.size())
		config.dynamicImageFormats->first.resize(index + 1);

	config.dynamicImageFormats->first.at(index) = format;
	return *this;
}

Vk::GraphicsPipelineBuilder& Vk::GraphicsPipelineBuilder::SetDepthAttachmentFormat(VkFormat format)
{
	if (!config.dynamicImageFormats.has_value())
		config.dynamicImageFormats = std::pair<std::vector<VkFormat>, VkFormat>();

	config.dynamicImageFormats->second = format;
	return *this;
}

std::shared_ptr<Vk::GraphicsPipeline> Vk::GraphicsPipelineBuilder::Build(VkRenderPass renderPass, uint32_t subpassIndex)
{
	config.renderPass = renderPass;
	config.subpassIndex = subpassIndex;
	return std::make_shared<GraphicsPipeline>(config);
}

std::shared_ptr<Vk::GraphicsPipeline> Vk::GraphicsPipelineBuilder::BuildDynamic()
{
	return std::make_shared<GraphicsPipeline>(config);
}