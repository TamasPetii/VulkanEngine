#version 460
#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_buffer_reference : require
#extension GL_GOOGLE_include_directive : require

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer { 
	Vertex vertices[];
};

layout( push_constant ) uniform constants
{	
	VertexBuffer vertexBuffer;
} PushConstants;

void main() 
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	gl_Position = vec4(v.position, 1.0f);

	vs_out_pos = v.position;
	vs_out_normal = v.normal;
	vs_out_tex = vec2(v.uv_x, v.uv_y);
}