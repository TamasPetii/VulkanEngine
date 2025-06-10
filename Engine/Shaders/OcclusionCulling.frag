#version 460

#include "Common/Render/Occlusion.glsl"

layout(location = 0) in flat uint fs_in_id;
layout(location = 0) out vec4 fs_out_col;

layout( push_constant ) uniform constants
{	
	uvec2 cameraBuffer;
	uvec2 transformBufferAddress;
	uvec2 instanceBufferAddress;
	uvec2 vertexBufferAddress;
	uvec2 indexBufferAddress;
	uvec2 occlusionBufferAddress;
	vec3 padding;
	uint cameraIndex;
} PushConstants;

void main()
{
	OcclusionBuffer(PushConstants.occlusionBufferAddress).indices[fs_in_id] = 1;
	fs_out_col = vec4(10, 10, 10, 1);
}