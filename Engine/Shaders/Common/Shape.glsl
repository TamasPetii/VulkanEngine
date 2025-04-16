#extension GL_EXT_buffer_reference : require

struct ShapeData
{
	uint entityIndex;
	uint transformIndex;
	uint materialIndex;
	uint receiveShadow;
};

layout(buffer_reference, std430) readonly buffer ShapeBuffer {
	ShapeData shapeDatas[];
};