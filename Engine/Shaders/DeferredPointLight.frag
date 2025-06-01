#version 460

#include "Common/Camera.glsl"
#include "Common/Light/PointLight.glsl"

layout(location = 0) in flat uint fs_in_id;
layout(location = 0) out vec4 fs_out_col;

layout(set = 0, binding = 1) uniform sampler2D u_positionTexture;
layout(set = 0, binding = 2) uniform sampler2D u_colorTexture;
layout(set = 0, binding = 3) uniform sampler2D u_normalTexture;

layout( push_constant ) uniform constants
{	
	uvec2 cameraBuffer;
	uvec2 pointLightBuffer;
	uvec2 transformBufferAddress;
	uvec2 instanceBufferAddress;
	uvec2 vertexBufferAddress;
	uvec2 indexBufferAddress;
	vec2 viewPortSize;
	uint cameraIndex;
	uint padding;
} PushConstants;

void main()
{
	vec2 fs_in_tex = gl_FragCoord.xy / PushConstants.viewPortSize;
	vec3 position = texture(u_positionTexture, fs_in_tex).xyz;

	PointLight light = PointLightBuffer(PushConstants.pointLightBuffer).lights[fs_in_id]; //With viewProj will be bad..., to much data
		
	if(distance(position, light.position) > light.radius)
		discard;

    vec3 color = texture(u_colorTexture, fs_in_tex).xyz;
	vec3 normal = texture(u_normalTexture, fs_in_tex).xyz;

	vec3 toLight = normalize(light.position - position);

	float cosa = clamp(dot(normal, toLight), 0, 1);
	fs_out_col = vec4(cosa * light.color, 1);
}