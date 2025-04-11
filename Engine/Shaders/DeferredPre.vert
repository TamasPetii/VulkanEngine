#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#define NORMAL_INSTANCED = 0
#define MULTIDRAW_INDIRECT_INSTANCED = 1

#include "Common/Vertex.glsl"
#include "Common/Transform.glsl"
#include "Common/Camera.glsl"
#include "Common/Material.glsl"
#include "Common/Incides.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;
layout (location = 3) out flat uvec2 vs_out_index;

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBuffer;
	uvec2 vertexBuffer;
	uvec2 indexBuffer;
	uvec2 transformBuffer;
	uvec2 materialBuffer;
	uvec2 materialIndexBuffer;
} PushConstants;

void main() 
{
	Vertex v = VertexBuffer(PushConstants.vertexBuffer).vertices[gl_VertexIndex];

	gl_Position = CameraBuffer(PushConstants.cameraBuffer).cameras[PushConstants.cameraIndex].viewProj * TransformBuffer(PushConstants.transformBuffer).transforms[IndexBuffer(PushConstants.indexBuffer).indices[gl_InstanceIndex].y].transform * vec4(v.position, 1.0);

	vs_out_pos = v.position;
	vs_out_normal = v.normal;
	vs_out_tex = vec2(v.uv_x, v.uv_y);
	vs_out_index.x = IndexBuffer(PushConstants.indexBuffer).indices[gl_InstanceIndex].x; //Entity
	vs_out_index.y = PushConstants.renderMode == NORMAL_INSTANCED ? IndexBuffer(PushConstants.indexBuffer).indices[gl_InstanceIndex].z : gl_DrawID;
}