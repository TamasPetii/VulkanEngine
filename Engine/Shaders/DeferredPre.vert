#version 460
#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_buffer_reference : require

#include "Common/Vertex.glsl"
#include "Common/Transform.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;

layout( push_constant ) uniform constants
{	
	mat4 viewProj;
	VertexBuffer vertexBuffer;
	TransformBuffer transformBuffer;
} PushConstants;

void main() 
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	//gl_Position = vec4(v.position + vec3(2 * (gl_InstanceIndex / 64), 0, 2 * (gl_InstanceIndex % 64)), 1.0f);
	gl_Position = PushConstants.viewProj * PushConstants.transformBuffer.transforms[gl_InstanceIndex].transform * vec4(v.position, 1.0f);

	vs_out_pos = v.position;
	vs_out_normal = v.normal;
	vs_out_tex = vec2(v.uv_x, v.uv_y);
}