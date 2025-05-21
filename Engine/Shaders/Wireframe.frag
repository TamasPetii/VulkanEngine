#version 460

layout(location = 0) out vec4 fs_out_col;

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBufferAddress;
	uvec2 transformBufferAddress;
	uvec2 vertexBufferAddress;
	uvec2 indexBufferAddress;
	uvec2 padding;
	vec4 color;
} PushConstants;

void main()
{
    fs_out_col = PushConstants.color;
}