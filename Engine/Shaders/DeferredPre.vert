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
#include "Common/VertexBone.glsl"

layout (location = 0) out vec3 vs_out_pos;
layout (location = 1) out vec3 vs_out_norm;
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
	uvec2 vertexBoneBuffer;
} PushConstants;

void main() 
{
	Vertex v = VertexBuffer(PushConstants.vertexBuffer).vertices[gl_VertexIndex];
	RenderIndices indices = RenderIndicesBuffer(PushConstants.renderIndicesBuffer).indices[InstanceIndexBuffer(PushConstants.instanceIndexBuffer).indices[gl_InstanceIndex]];

	vec4 position = vec4(v.position, 1.0);
	vec4 normal = TransformBuffer(PushConstants.transformBuffer).transforms[indices.transformIndex].transformIT * vec4(v.normal, 0.0);
	vec4 tangent = TransformBuffer(PushConstants.transformBuffer).transforms[indices.transformIndex].transformIT * vec4(v.tangent, 0.0);
	vec4 bitangent = TransformBuffer(PushConstants.transformBuffer).transforms[indices.transformIndex].transformIT * vec4(v.bitangent, 0.0);

	if(PushConstants.renderMode == MODEL_INSTANCED)
	{	
		bool hasBone = false;
		//Animation index is packed into materialIndex
		if(indices.animationIndex != INVALID_RENDER_INDEX)
		{
			VertexBone vertexBone = VertexBoneBuffer(PushConstants.vertexBoneBuffer).vertexBones[gl_VertexIndex];	
			hasBone = vertexBone.indices[0] != INVALID_VERTEX_BONE_INDEX;

			if(hasBone)
			{
				vec4 totalPosition = vec4(0);
				vec4 totalNormal = vec4(0);
				vec4 totalTanget = vec4(0);
				vec4 totalBitangent = vec4(0);

				for(int i = 0; i < 4; i++)
				{
					if(vertexBone.indices[i] == INVALID_VERTEX_BONE_INDEX)
					   continue;

					totalPosition += (NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[vertexBone.indices[i]].transform * position) * vertexBone.weights[i];
					totalNormal += (NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[vertexBone.indices[i]].transformIT * normal) * vertexBone.weights[i];
					totalTanget += (NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[vertexBone.indices[i]].transformIT * tangent) * vertexBone.weights[i];
					totalBitangent += (NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[vertexBone.indices[i]].transformIT * bitangent) * vertexBone.weights[i];
				}
					
				position = totalPosition;
				normal = totalNormal;
				tangent = totalTanget;
				bitangent = totalBitangent;
			}
		}
		
		if(!hasBone)
		{
			position = NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[v.nodeIndex].transform * position;
			normal = NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[v.nodeIndex].transformIT * normal;
			tangent = NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[v.nodeIndex].transformIT * tangent;
			bitangent = NodeTransformBuffer(PushConstants.nodeTransformBuffer).transforms[v.nodeIndex].transformIT * bitangent;
		}
	}

	vec4 worldPosition = TransformBuffer(PushConstants.transformBuffer).transforms[indices.transformIndex].transform * position;
	gl_Position = CameraBuffer(PushConstants.cameraBuffer).cameras[PushConstants.cameraIndex].viewProj * worldPosition;

	vec3 finalNormal = normalize(normal.xyz);
	vec3 finalTangent = normalize(tangent.xyz);
	finalTangent = normalize(finalTangent - dot(finalTangent, finalNormal) * finalNormal);
	vec3 finalBitangent = normalize(cross(finalNormal, finalTangent));
	//finalBitangent = normalize(bitangent.xyz);

	vs_out_pos = worldPosition.xyz;
	vs_out_norm = finalNormal;
	vs_out_tex = vec2(v.uv_x, 1.0 - v.uv_y);
	vs_out_index.x = indices.entityIndex;
	vs_out_index.y = PushConstants.renderMode == SHAPE_INSTANCED ? indices.materialIndex : v.matIndex; //Material Index
	vs_out_index.z = indices.flag;
	vs_out_tbn = mat3(finalTangent, finalBitangent, finalNormal);
}