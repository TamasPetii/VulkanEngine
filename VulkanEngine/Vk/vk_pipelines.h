#pragma once
#include <fstream>
#include "vk_initializers.h"
#include "vk_types.h"

class Vkpipelines
{
public:
	static bool LoadShaderModule(const std::string& filePath, VkDevice logicalDevice, VkShaderModule* outShaderModule);
};

class PipelineBuilder
{
public:
    PipelineBuilder();
    VkPipeline BuildPipeline(VkDevice logicalDevice);
	void Clear();
    void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
    void SetInputTopology(VkPrimitiveTopology topology);
    void SetPolygonMode(VkPolygonMode mode);
    void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
    void SetMultisamplingNone();
    void DisableBlending();
    void SetColorAttachmentFormat(VkFormat format);
    void SetDepthFormat(VkFormat format);
    void DisableDepthTest();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineLayout pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineRenderingCreateInfo renderInfo;
    VkFormat colorAttachmentformat;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
};