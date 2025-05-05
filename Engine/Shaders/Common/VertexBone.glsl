#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require

#define INVALID_VERTEX_BONE_INDEX 0xFFFFFFFFu

struct VertexBone {
	uvec4 indices;
	vec4 weights;
}; 

layout(buffer_reference, std430) readonly buffer VertexBoneBuffer { 
	VertexBone vertexBones[];
};

layout(buffer_reference, std430) readonly buffer AnimationVertexBoneBuffers { 
	uvec2 animationVertexBonebufferAddresses[];
};