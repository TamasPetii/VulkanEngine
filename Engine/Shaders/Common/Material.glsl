#extension GL_EXT_buffer_reference : require

#define INVALID_IMAGE_INDEX 0xFFFFFFFFu

struct Material {
	vec4 color;
	vec2 uvScale;
	vec2 bloom;
	uint albedoIndex;
	uint normalIndex;
	uint metallicIndex;
	uint roughnessIndex;
}; 

layout(buffer_reference, std430) readonly buffer MaterialBuffer { 
	Material materials[];
};

layout(buffer_reference, std430) readonly buffer MaterialIndexBuffer { 
	uint materialIndices[];
};