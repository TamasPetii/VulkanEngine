#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require

struct NodeTransform {
	mat4 transform;
	mat4 transformIT;
}; 

layout(buffer_reference, std430) readonly buffer NodeTransformBuffer { 
	NodeTransform transforms[];
};

layout(buffer_reference, std430) readonly buffer NodeTransformBuffers { 
	uvec2 nodeTransformBufferAddresses[];
};