#include "ShaderModule.h"

Vk::ShaderModule::ShaderModule(const std::string& filePath, VkShaderStageFlagBits shaderStage, const std::string& functionName) : 
    shaderFilePath(filePath),
    mainFunctionName(functionName)
{
    Init(shaderStage);
}

Vk::ShaderModule::~ShaderModule()
{
    Destroy();
}

const VkPipelineShaderStageCreateInfo& Vk::ShaderModule::GetPipelineShaderStageInfo() const
{
    return shaderStageInfo;
}

void Vk::ShaderModule::Init(VkShaderStageFlagBits shaderStage)
{
    auto device = VulkanContext::GetContext()->GetDevice();
    auto shaderBinaryData = ShaderCompiler::Compile(shaderFilePath, ConvertShaderFlagToShaderc(shaderStage));

    VkShaderModuleCreateInfo moduleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = shaderBinaryData.size() * sizeof(uint32_t),
        .pCode = shaderBinaryData.data(),
    };

    VK_CHECK_MESSAGE(vkCreateShaderModule(device->Value(), &moduleCreateInfo, nullptr, &shaderModule), "Failed to create shader module!")

    shaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = shaderStage,
        .module = shaderModule,
        .pName = mainFunctionName.c_str()
    };
}

void Vk::ShaderModule::Destroy()
{
    auto device = VulkanContext::GetContext()->GetDevice();

    if(shaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(device->Value(), shaderModule, nullptr);

    shaderModule = VK_NULL_HANDLE;
}

shaderc_shader_kind Vk::ShaderModule::ConvertShaderFlagToShaderc(VkShaderStageFlagBits shaderStage)
{
    switch (shaderStage)
    {
    case VK_SHADER_STAGE_VERTEX_BIT:
        return shaderc_vertex_shader;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
        return shaderc_fragment_shader;
    case VK_SHADER_STAGE_COMPUTE_BIT:
        return shaderc_compute_shader;
    case VK_SHADER_STAGE_GEOMETRY_BIT:
        return shaderc_geometry_shader;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return shaderc_tess_control_shader;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return shaderc_tess_evaluation_shader;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
        return shaderc_raygen_shader;
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
        return shaderc_anyhit_shader;
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
        return shaderc_closesthit_shader;
    case VK_SHADER_STAGE_MISS_BIT_KHR:
        return shaderc_miss_shader;
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
        return shaderc_intersection_shader;
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
        return shaderc_callable_shader;
    case VK_SHADER_STAGE_TASK_BIT_EXT:
        return shaderc_task_shader;
    case VK_SHADER_STAGE_MESH_BIT_EXT:
        return shaderc_mesh_shader;
    default:
        return shaderc_glsl_infer_from_source;
    }
}
