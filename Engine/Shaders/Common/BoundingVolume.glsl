#extension GL_EXT_buffer_reference : require

layout(buffer_reference, std430) readonly buffer BoundingVolumeBuffer { 
	mat4 transforms[];
};