#extension GL_EXT_buffer_reference : require

layout(buffer_reference, std430) readonly buffer InstanceIndexBuffer {
	uvec4 indices[]; //Entity Index | Transform Index | Material Index | Filler
};