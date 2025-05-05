#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#define SHAPE_INSTANCED 0
#define MODEL_INSTANCED 1
#define MULTIDRAW_INDIRECT_INSTANCED 2

struct GeometryRendererPushConstants
{
	uint32_t renderMode;
	uint32_t cameraIndex;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress vertexBuffer;
	VkDeviceAddress instanceIndexBuffer;
	VkDeviceAddress transformBuffer;
	VkDeviceAddress materialBuffer;
	VkDeviceAddress renderIndicesBuffer;
	VkDeviceAddress nodeTransformBuffers; 
	VkDeviceAddress animationVertexBoneBuffers;
};

struct BoundingVolumeRendererPushConstants
{
	uint32_t renderMode;
	uint32_t cameraIndex;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress vertexBuffer;
	VkDeviceAddress transformBuffer;
	glm::vec4 color;
};