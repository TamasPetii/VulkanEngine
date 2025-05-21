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
	VkDeviceAddress transformBuffer;
	VkDeviceAddress instanceIndexBuffer;
	VkDeviceAddress renderIndicesBuffer;
	VkDeviceAddress modelBufferAddresses;
	VkDeviceAddress animationTransformBufferAddresses;
	VkDeviceAddress animationVertexBoneBufferAddresses;
};

struct DeferredDirectionLightPushConstants
{	
	uint32_t cameraIndex;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress directionLightBufferAddress;
};

struct DeferredPointLightPushConstants
{
	uint32_t cameraIndex;
	VkDeviceAddress cameraBuffer;
	VkDeviceAddress pointLightBufferAddress;
	VkDeviceAddress transformBufferAddress;
	VkDeviceAddress vertexBufferAddress;
	VkDeviceAddress indexBufferAddress;
	glm::vec2 viewPortSize;
};

struct BoundingVolumeRendererPushConstants
{
	uint32_t renderMode;
	uint32_t cameraIndex;
	VkDeviceAddress cameraBufferAddress;
	VkDeviceAddress transformBufferAddress;
	VkDeviceAddress vertexBufferAddress;
	VkDeviceAddress indexBufferAddress;
	VkDeviceAddress padding;
	glm::vec4 color;
};