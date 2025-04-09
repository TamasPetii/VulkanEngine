#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct GpuPushConstant
{
	uint32_t entityIndex;
	uint32_t cameraIndex;
	VkDeviceAddress vertexBuffer;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress transformBuffer;
};