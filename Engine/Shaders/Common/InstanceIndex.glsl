#extension GL_EXT_buffer_reference : require

layout(buffer_reference, std430) readonly buffer InstanceIndexBuffer {
	uint indices[]; //Index into shape/model data buffer
};