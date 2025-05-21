#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require
#define INVALID_RENDER_INDEX 0xFFFFFFFFu

struct RenderIndices
{
	uint entityIndex;
	uint transformIndex;
	uint modelIndex; //This is the loaded model array index! This is not the model component dense index!!
	uint animationIndex;
	uint animationTransformIndex;
	uint bitflag;
};

layout(buffer_reference, std430) readonly buffer RenderIndicesBuffer {
	RenderIndices indices[];
};