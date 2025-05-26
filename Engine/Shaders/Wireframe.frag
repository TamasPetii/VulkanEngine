#version 460

layout(location = 0) out vec4 fs_out_col;

layout( push_constant ) uniform constants
{	
	uvec2 cameraBufferAddress;
	uvec2 transformBufferAddress;
	uvec2 vertexBufferAddress;
	uvec2 indexBufferAddress;
	vec4 color;
	vec3 padding;
	uint cameraIndex;
} PushConstants;

void main()
{
    fs_out_col = PushConstants.color;
}