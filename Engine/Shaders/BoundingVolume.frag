#version 460

layout(location = 0) out vec4 fs_out_col;

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBuffer;
	uvec2 vertexBuffer;
	uvec2 transformBuffer;
	vec4 color;
} PushConstants;

void main()
{
    fs_out_col = PushConstants.color;
}