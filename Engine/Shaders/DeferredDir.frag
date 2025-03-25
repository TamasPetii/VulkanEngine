#version 460
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 fs_in_tex;
layout(location = 0) out vec4 fs_out_col;

layout(set = 0, binding = 0) uniform sampler2D u_mainTexture;
layout(set = 0, binding = 1) uniform sampler2D u_colorTexture;
layout(set = 0, binding = 2) uniform sampler2D u_normalTexture;

void main()
{
	fs_out_col = vec4(texture(u_colorTexture, fs_in_tex).xyz, 1);
}