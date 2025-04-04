#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct GpuPushConstant
{
	glm::mat4 viewProj;
	VkDeviceAddress vertexBuffer;
	VkDeviceAddress transformBuffer;
};