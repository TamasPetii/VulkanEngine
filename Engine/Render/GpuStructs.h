#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#define NORMAL_INSTANCED 0
#define MULTIDRAW_INDIRECT_INSTANCED 1

struct GpuPushConstant
{
	uint32_t renderMode;
	uint32_t cameraIndex;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress vertexBuffer;
	VkDeviceAddress instanceIndexBuffer;
	VkDeviceAddress transformBuffer;
	VkDeviceAddress materialBuffer;
	VkDeviceAddress shapeBuffer;
};