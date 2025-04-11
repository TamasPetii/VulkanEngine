#extension GL_EXT_buffer_reference : require

struct Transform {
	mat4 transform;
	mat4 transformIT;
}; 

layout(buffer_reference, std430) readonly buffer TransformBuffer { 
	Transform transforms[];
};