#pragma once
#include "../EngineApi.h"
#include "VulkanContext.h"
#include "ShaderCompiler.h"
#include <string>
#include <vector>

namespace Vk
{
	class ENGINE_API ShaderModule
	{
	public:
		ShaderModule(const std::string& filePath, VkShaderStageFlagBits shaderStage, const std::string& functionName = "main");
		~ShaderModule();
		const VkPipelineShaderStageCreateInfo& GetPipelineShaderStageInfo() const;
	private:	
		void Init(VkShaderStageFlagBits shaderStage);
		void Destroy();
		shaderc_shader_kind ConvertShaderFlagToShaderc(VkShaderStageFlagBits shaderStage);
	private:
		std::string shaderFilePath;
		std::string mainFunctionName;
		VkShaderModule shaderModule = VK_NULL_HANDLE;
		VkPipelineShaderStageCreateInfo shaderStageInfo;
	};
}