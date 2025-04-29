#version 460
#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) in vec2 fs_in_tex;
layout(location = 0) out vec4 fs_out_col;

layout(set = 0, binding = 1) uniform sampler2D u_colorTexture;
layout(set = 0, binding = 2) uniform sampler2D u_normalTexture;

void main()
{
    vec3 color = texture(u_colorTexture, fs_in_tex).xyz;
	vec3 normal = texture(u_normalTexture, fs_in_tex).xyz;

    vec3 lightDir = vec3(-0.9, -0.5, -0.8);
	float cosa = clamp(dot(normal, -lightDir), 0, 1);

	fs_out_col = vec4(cosa * color, 1);
}