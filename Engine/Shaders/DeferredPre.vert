#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/RenderDefines.glsl"
#include "Common/Vertex.glsl"
#include "Common/Transform.glsl"
#include "Common/Camera.glsl"
#include "Common/Material.glsl"
#include "Common/InstanceIndex.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;
layout (location = 3) out flat uvec2 vs_out_index;
layout (location = 4) out mat3 vs_out_tbn;

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBuffer;
	uvec2 vertexBuffer;
	uvec2 instanceIndexBuffer;
	uvec2 transformBuffer;
	uvec2 materialBuffer;
} PushConstants;

void main() 
{
	Vertex v = VertexBuffer(PushConstants.vertexBuffer).vertices[gl_VertexIndex];
	uvec4 indices = InstanceIndexBuffer(PushConstants.instanceIndexBuffer).indices[gl_InstanceIndex];

	gl_Position = CameraBuffer(PushConstants.cameraBuffer).cameras[PushConstants.cameraIndex].viewProj * TransformBuffer(PushConstants.transformBuffer).transforms[indices.y].transform * vec4(v.position, 1.0);

	vec3 normal = normalize(vec3(TransformBuffer(PushConstants.transformBuffer).transforms[indices.y].transformIT * vec4(v.normal, 0)));
	vec3 tangent = normalize(vec3(TransformBuffer(PushConstants.transformBuffer).transforms[indices.y].transformIT * vec4(v.tangent, 0)));
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = normalize(cross(normal, tangent));

	vs_out_pos = v.position;
	vs_out_normal = normal;
	vs_out_tex = vec2(v.uv_x, v.uv_y);
	vs_out_index.x = indices.x; //Entity ID
	vs_out_index.y = PushConstants.renderMode == NORMAL_INSTANCED ? indices.z : v.index; //Material Index
	vs_out_tbn = mat3(tangent, bitangent, normal);
}