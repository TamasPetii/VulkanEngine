#version 460
#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_buffer_reference : require
#extension GL_GOOGLE_include_directive : require

layout(location = 0) out vec2 vs_out_tex;

vec4 positions[4] = vec4[4](
	vec4(-1,-1, 0, 1),
	vec4( 1,-1, 0, 1),
	vec4(-1, 1, 0, 1),
	vec4( 1, 1, 0, 1)
);

vec2 texCoords[4] = vec2[4](
	vec2(0,0),
	vec2(1,0),
	vec2(0,1),
	vec2(1,1)
);

void main() 
{
	gl_Position = positions[gl_VertexIndex];
	vs_out_tex	= texCoords[gl_VertexIndex];
}