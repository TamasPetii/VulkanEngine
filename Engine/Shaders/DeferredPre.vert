#version 460
#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_buffer_reference : require

#include "Common/Vertex.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;

layout( push_constant ) uniform constants
{	
	mat4 viewProj;
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	gl_Position = PushConstants.viewProj * vec4(v.position + vec3(2 * (gl_InstanceIndex / 64), 0, 2 * (gl_InstanceIndex % 64)), 1.0f);

	vs_out_pos = v.position;
	vs_out_normal = v.normal;
	vs_out_tex = vec2(v.uv_x, v.uv_y);
}