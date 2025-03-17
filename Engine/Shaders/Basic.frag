#version 460

//Inputs
layout (location = 0) in vec3 fs_in_pos;
layout (location = 1) in vec3 fs_in_normal;
layout (location = 2) in vec3 fs_in_color;
layout (location = 3) in vec2 fs_in_tex;

//Outputs
layout (location = 0) out vec4 fs_out_color;

void main() 
{
	fs_out_color = vec4(fs_in_color, 1.0f);
}