#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/RenderIndices.glsl"
#include "Common/RenderDefines.glsl"
#include "Common/Vertex.glsl"
#include "Common/Transform.glsl"
#include "Common/Camera.glsl"
#include "Common/Material.glsl"
#include "Common/InstanceIndex.glsl"
#include "Common/NodeTransform.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_normal;
layout (location = 2) out vec2 vs_out_tex;
layout (location = 3) out flat uvec3 vs_out_index;
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
	uvec2 renderIndicesBuffer;
	uvec2 nodeTransformBuffer;
} PushConstants;

void main() 
{
	Vertex v = VertexBuffer(PushConstants.vertexBuffer).vertices[gl_VertexIndex];
	RenderIndices indices = RenderIndicesBuffer(PushConstants.renderIndicesBuffer).indices[InstanceIndexBuffer(PushConstants.instanceIndexBuffer).indices[gl_InstanceIndex]];

	mat4 localTransform = TransformBuffer(PushConstants.transformBuffer).transforms[indices.transformIndex].transform;
	mat4 localTransformIT = TransformBuffer(PushConstants.transformBuffer).transforms[indices.transformIndex].transformIT;

	if(PushConstants.renderMode == MODEL_INSTANCED)
	{	
		localTransform = localTransform * NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[v.nodeIndex].transform;
		localTransformIT = localTransformIT * NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[v.nodeIndex].transformIT;
	}

	vec4 worldPosition = localTransform * vec4(v.position, 1.0);
	gl_Position = CameraBuffer(PushConstants.cameraBuffer).cameras[PushConstants.cameraIndex].viewProj * worldPosition;

	vec3 normal = normalize(vec3(localTransformIT * vec4(v.normal, 0)));
	vec3 tangent = normalize(vec3(localTransformIT * vec4(v.tangent, 0)));
	vec3 bitangent = normalize(vec3(localTransformIT * vec4(v.bitangent, 0)));

	vs_out_pos = worldPosition.xyz;
	vs_out_normal = normal;
	vs_out_tex = vec2(v.uv_x, 1.0 - v.uv_y);
	vs_out_index.x = indices.entityIndex; //Entity ID
	vs_out_index.y = PushConstants.renderMode == SHAPE_INSTANCED ? indices.materialIndex : v.matIndex; //Material Index
	vs_out_index.z = indices.flag;
	vs_out_tbn = mat3(tangent, bitangent, normal);
}