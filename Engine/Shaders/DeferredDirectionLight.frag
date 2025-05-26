#version 460

#include "Common/Camera.glsl"
#include "Common/Light/DirectionLight.glsl"

layout(location = 0) in vec2 fs_in_tex;
layout(location = 1) in flat uint fs_in_id;

layout(location = 0) out vec4 fs_out_col;

layout(set = 0, binding = 1) uniform sampler2D u_positionTexture;
layout(set = 0, binding = 2) uniform sampler2D u_colorTexture;
layout(set = 0, binding = 3) uniform sampler2D u_normalTexture;

layout( push_constant ) uniform constants
{	
	uvec2 cameraBuffer;
	uvec2 directionLightBuffer;
	vec3 padding;
	uint cameraIndex;
} PushConstants;

void main()
{
	vec3 position = texture(u_positionTexture, fs_in_tex).xyz;
    vec3 color = texture(u_colorTexture, fs_in_tex).xyz;
	vec3 normal = texture(u_normalTexture, fs_in_tex).xyz;

	//With viewProj will be bad..., too much data
	DirectionLight light = DirectionLightBuffer(PushConstants.directionLightBuffer).lights[fs_in_id];

	float cosa = clamp(dot(normal, -light.direction), 0, 1);
	fs_out_col = vec4(cosa * color * light.color * 1.25, 1);
}