#pragma once
#include "VulkanContext.h"
#include "ShaderModule.h"

namespace Vk
{
	struct ENGINE_API ComputePipelineConfig
	{
		VkPipelineShaderStageCreateInfo shaderStage;
		std::vector<VkPushConstantRange> pushConstants;
		std::vector<VkDescriptorSetLayout> descriptorLayouts;
	};

	class ENGINE_API ComputePipeline
	{
	public:
		ComputePipeline(const ComputePipelineConfig& config);
		~ComputePipeline();
		VkPipeline Pipeline() { return pipeline; }
		VkPipelineLayout Layout() { return layout; }
	private:
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	class ENGINE_API ComputePipelineBuilder
	{
	public:
		ComputePipelineBuilder& AddShaderStage(std::shared_ptr<ShaderModule> shader);
		ComputePipelineBuilder& AddPushConstant(uint32_t offset, uint32_t size, VkShaderStageFlags stages);
		ComputePipelineBuilder& AddDescriptorSetLayout(VkDescriptorSetLayout layout);
	private:
		ComputePipelineConfig config;
	};
}