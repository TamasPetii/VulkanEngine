#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct GpuPushConstant
{
	uint32_t cameraIndex;
	uint32_t transformIndex;
	VkDeviceAddress vertexBuffer;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress transformBuffer;
};