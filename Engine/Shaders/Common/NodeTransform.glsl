#extension GL_EXT_buffer_reference : require

struct NodeTransform {
	mat4 transform;
	mat4 transformIT;
}; 

layout(buffer_reference, std430) readonly buffer NodeTransformBuffer { 
	NodeTransform transforms[];
};