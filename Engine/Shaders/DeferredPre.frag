#version 460
#extension GL_KHR_vulkan_glsl : enable

#include "Common/Images.glsl"

//Inputs
layout (location = 0) in vec3 fs_in_pos;
layout (location = 1) in vec3 fs_in_normal;
layout (location = 2) in vec2 fs_in_tex;
layout (location = 3) in flat uint fs_in_entity;

//Outputs
layout (location = 0) out vec4 fs_out_color;
layout (location = 1) out vec4 fs_out_normal;
layout (location = 2) out uint fs_out_entity;

void main() 
{
    vec4 color = sampleTexture2D(0, NEAREST_SAMPLER_ID, fs_in_tex);
	fs_out_color = vec4(color.xyz, 1);
	fs_out_normal = vec4(normalize(fs_in_normal), 1);
	fs_out_entity = fs_in_entity;
}