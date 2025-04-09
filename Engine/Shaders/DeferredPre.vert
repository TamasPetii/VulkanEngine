#version 460
#extension GL_KHR_vulkan_glsl : enable
#extension GL_EXT_buffer_reference : require

#include "Common/Vertex.glsl"
#include "Common/Transform.glsl"
#include "Common/Camera.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;
layout (location = 3) out flat uint vs_out_entity;

layout( push_constant ) uniform constants
{	
	uint entityIndex;
	uint cameraIndex;
	VertexBuffer vertexBuffer;
	CameraBuffer cameraBuffer;
	TransformBuffer transformBuffer;
} PushConstants;

void main() 
{
	Vertex v = PushConstants.vertexBuffer.vertices[gl_VertexIndex];

	gl_Position = PushConstants.cameraBuffer.cameras[PushConstants.cameraIndex].viewProj * PushConstants.transformBuffer.transforms[gl_InstanceIndex].transform * vec4(v.position, 1.0f);

	vs_out_pos = v.position;
	vs_out_normal = v.normal;
	vs_out_tex = vec2(v.uv_x, v.uv_y);
	vs_out_entity = PushConstants.entityIndex;
}