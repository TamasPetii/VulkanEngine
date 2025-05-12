#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require

struct AnimationNodeTransform {
	mat4 transform;
	mat4 transformIT;
}; 

layout(buffer_reference, std430) readonly buffer AnimationNodeTransformBuffer { 
	AnimationNodeTransform nodeTransforms[];
};