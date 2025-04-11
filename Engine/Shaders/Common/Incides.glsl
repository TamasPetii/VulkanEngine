#extension GL_EXT_buffer_reference : require

layout(buffer_reference, std430) readonly buffer IndexBuffer { //ID BUFFER -> Not usual index buffer
	uvec4 indices[];
};

//Entity Index | Transform Index | Material Index