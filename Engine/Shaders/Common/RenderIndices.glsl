#extension GL_EXT_buffer_reference : require

#define INVALID_RENDER_INDEX 0xFFFFFFFFu

struct RenderIndices
{
	uint entityIndex;
	uint transformIndex;
	uint materialIndex;
	uint flag;
	uint nodeTransformIndex;
	uint animationIndex;
	uint filler1;
	uint filler2;
};

layout(buffer_reference, std430) readonly buffer RenderIndicesBuffer {
	RenderIndices indices[];
};