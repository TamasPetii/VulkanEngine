#pragma once
#include "VulkanContext.h"
#include "ShaderModule.h"
#include "RenderPass.h"
#include <string>
#include <vector>
#include <array>

namespace Vk
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyInfo, const VkPipelineDynamicStateCreateInfo& dynamicStateInfo, const VkPipelineViewportStateCreateInfo& viewportStateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationInfo, const VkPipelineMultisampleStateCreateInfo& multisamplingInfo, const VkPipelineColorBlendAttachmentState& colorBlendAttachmentInfo, const VkPipelineColorBlendStateCreateInfo& colorBlendInfo, const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo, const std::span<VkPushConstantRange> pushConstantInfos, const std::span<VkDescriptorSetLayout> descriptorLayoutInfos, const std::span<VkPipelineShaderStageCreateInfo> shaderStageInfos, const VkPipelineRenderingCreateInfo* renderingInfo);
		~GraphicsPipeline();
		const VkPipeline Value() const;
	private:
		void Init(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyInfo, const VkPipelineDynamicStateCreateInfo& dynamicStateInfo, const VkPipelineViewportStateCreateInfo& viewportStateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationInfo, const VkPipelineMultisampleStateCreateInfo& multisamplingInfo, const VkPipelineColorBlendAttachmentState& colorBlendAttachmentInfo, const VkPipelineColorBlendStateCreateInfo& colorBlendInfo, const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo, const std::span<VkPushConstantRange> pushConstantInfos, const std::span<VkDescriptorSetLayout> descriptorLayoutInfos, const std::span<VkPipelineShaderStageCreateInfo> shaderStageInfos, const VkPipelineRenderingCreateInfo* renderingInfo);
		void Destroy();
		VkPipelineLayoutCreateInfo BuildPipelineLayoutInfo(const std::span<VkPushConstantRange> pushConstantInfos, const std::span<VkDescriptorSetLayout> descriptorLayoutInfos);
		VkGraphicsPipelineCreateInfo BuildGraphicsPipelineInfo(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex, const VkPipelineVertexInputStateCreateInfo& vertexInputInfo, const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyInfo,	const VkPipelineDynamicStateCreateInfo& dynamicStateInfo, const VkPipelineViewportStateCreateInfo& viewportStateInfo, const VkPipelineRasterizationStateCreateInfo& rasterizationInfo, const VkPipelineMultisampleStateCreateInfo& multisamplingInfo, const VkPipelineColorBlendAttachmentState& colorBlendAttachmentInfo, const VkPipelineColorBlendStateCreateInfo& colorBlendInfo, const VkPipelineDepthStencilStateCreateInfo& depthStencilInfo, const std::span<VkPipelineShaderStageCreateInfo> shaderStageInfos, const VkPipelineRenderingCreateInfo* renderingInfo);
	private:
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	};

	class GraphicsPipelineBuilder
	{
	public:
		void SetDefaultInfos();

		//Shader Stages (Vertex, Fragment, ...)
		void SetShaderStage(std::shared_ptr<ShaderModule> shaderModule);
		
		//Vertex Description
		void SetVertexInputInfo(std::span<VkVertexInputBindingDescription> bindingDescriptions = {}, std::span<VkVertexInputAttributeDescription> attributeDescriptions = {});
		
		//Input assembly (Triangle, line, ...)
		void SetInputAssemblyTopology(VkPrimitiveTopology topology);
		
		//Dynamic State
		void DefaultDynamicStates();
		void SetDynamicStates(std::span<VkDynamicState> dynamicStates);
		
		//Rasterization
		void DefaultRasterizationInfo();
		void SetRasterizationOptions(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, float lineWidth = 1.0f);
		
		//Color Blend
		void DefaultColorBlendOptions();
		void SetColorBlendOptions(VkBool32 enable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp AlphaBlendOp);

		//Depth operations
		void SetDepthStencilOptions(VkBool32 enableTest, VkBool32 enableWrite, VkCompareOp compare);

		//Uniform descriptors
		void SetPushConstantRange(uint32_t offset, uint32_t size, VkShaderStageFlags shaderStages);
		void SetDescriptorSetLayout(const VkDescriptorSetLayout& layout);

		//Dynamic Rendering Info
		void SetColorAttachmentFormats(std::span<VkFormat> imageFormats);
		void SetDepthAttachmentFormat(VkFormat depthFormat);

		std::shared_ptr<GraphicsPipeline> BuildPipeline(std::shared_ptr<Vk::RenderPass> renderPass, uint32_t subpassIndex);
		std::shared_ptr<GraphicsPipeline> BuildDynamicPipeline();
	private:
		void SetDefaultVertexInputInfo();
		void SetDefaultInputAssemblyInfo();
		void SetDefaultDynamicStateInfo();
		void SetDefaultRasterizationInfo();
		void SetDefaultViewPortStateInfo();
		void SetDefulatMultisamplingInfo();
		void SetDefaultColorBlendInfo();
		void SetDefaultDepthStencilInfo();
		void SetDefaultRenderingInfo();
	private:
		VkPipelineRenderingCreateInfo renderingInfo;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineViewportStateCreateInfo viewportStateInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisamplingInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkPushConstantRange> pushConstantInfos;
		std::vector<VkDescriptorSetLayout> descriptorLayoutInfos;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
	};
}
