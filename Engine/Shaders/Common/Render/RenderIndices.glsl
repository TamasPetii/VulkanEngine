#extension GL_EXT_buffer_reference : require
#define INVALID_RENDER_INDEX 0xFFFFFFFFu

struct RenderIndices
{
	uint entityIndex;
	uint transformIndex;
	uint modelIndex;
	uint animationIndex;
	uint animationTransformIndex;
	uint bitflag;
};

layout(buffer_reference, std430) readonly buffer RenderIndicesBuffer {
	RenderIndices indices[];
};