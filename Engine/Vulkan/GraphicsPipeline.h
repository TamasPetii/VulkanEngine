#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"
#include "ShaderModule.h"
#include "RenderPass.h"
#include <string>
#include <vector>
#include <array>

namespace Vk 
{
	struct ENGINE_API GraphicsPipelineConfig
	{
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};

		std::vector<VkDynamicState> dynamicStates;
		std::vector<VkPushConstantRange> pushConstants;
		std::vector<VkDescriptorSetLayout> descriptorLayouts;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

		std::optional<uint32_t> subpassIndex{};
		std::optional<VkRenderPass> renderPass{};
		std::optional<VkPipelineDepthStencilStateCreateInfo> depthStencilInfo{};
		std::optional<std::pair<std::vector<VkFormat>, VkFormat>> dynamicImageFormats{};
	};

	class ENGINE_API GraphicsPipeline
	{
	public:
		GraphicsPipeline(GraphicsPipelineConfig& config);
		~GraphicsPipeline();

		VkPipeline GetPipeline() const { return pipeline; }
		VkPipelineLayout GetLayout() const { return pipelineLayout; }
	private:
		bool Initialize(GraphicsPipelineConfig& config);
		void Cleanup();

		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	};

	class ENGINE_API GraphicsPipelineBuilder
	{
	public:
		GraphicsPipelineBuilder();
		Vk::GraphicsPipelineBuilder& ResetToDefault();

		// Core pipeline stages
		GraphicsPipelineBuilder& SetVertexInput(std::span<VkVertexInputBindingDescription> bindings, std::span<VkVertexInputAttributeDescription> attributes);
		GraphicsPipelineBuilder& SetPrimitiveTopology(VkPrimitiveTopology topology, VkBool32 primitiveRestart = false);

		// State configuration
		GraphicsPipelineBuilder& SetRasterization(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth = 1.0f);
		GraphicsPipelineBuilder& SetDepthStencil(VkBool32 depthTest, VkBool32 depthWrite, VkCompareOp depthCompare, VkBool32 stencilTest = false);
		GraphicsPipelineBuilder& AddDynamicState(VkDynamicState state);
		GraphicsPipelineBuilder& SetMultisampling(VkSampleCountFlagBits samples);

		// Blending
		GraphicsPipelineBuilder& SetColorBlend(VkBool32 enable, VkLogicOp operation = VK_LOGIC_OP_COPY);
		GraphicsPipelineBuilder& AddColorBlendAttachment(VkBool32 blendEnable, VkBlendFactor srcColor = VK_BLEND_FACTOR_ONE, VkBlendFactor dstColor = VK_BLEND_FACTOR_ZERO, VkBlendOp colorOp = VK_BLEND_OP_ADD, VkBlendFactor srcAlpha = VK_BLEND_FACTOR_ONE, VkBlendFactor dstAlpha = VK_BLEND_FACTOR_ZERO, VkBlendOp alphaOp = VK_BLEND_OP_ADD);

		// Layout
		GraphicsPipelineBuilder& AddPushConstant(uint32_t offset, uint32_t size, VkShaderStageFlags stages);
		GraphicsPipelineBuilder& AddDescriptorSetLayout(VkDescriptorSetLayout layout);

		// Dynamic rendering specific
		GraphicsPipelineBuilder& SetColorAttachmentFormats(VkFormat format, uint32_t index);
		GraphicsPipelineBuilder& SetDepthAttachmentFormat(VkFormat format);

		// Build methods
		std::shared_ptr<GraphicsPipeline> Build(VkRenderPass renderPass, uint32_t subpassIndex);
		std::shared_ptr<GraphicsPipeline> BuildDynamic();

	private:
		GraphicsPipelineConfig config;
	};
}