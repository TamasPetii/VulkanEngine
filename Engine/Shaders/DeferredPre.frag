#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/RenderDefines.glsl"
#include "Common/Images.glsl"
#include "Common/Material.glsl"

//Inputs
layout (location = 0) in vec3 fs_in_pos;
layout (location = 1) in vec3 fs_in_normal;
layout (location = 2) in vec2 fs_in_tex;
layout (location = 3) in flat uvec3 fs_in_index;
layout (location = 4) in mat3 fs_in_tbn;

//Outputs
layout (location = 0) out vec4 fs_out_pos;
layout (location = 1) out vec4 fs_out_color;
layout (location = 2) out vec4 fs_out_normal;
layout (location = 3) out uint fs_out_entity;

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBuffer;
	uvec2 vertexBuffer;
	uvec2 indexBuffer;
	uvec2 transformBuffer;
	uvec2 materialBuffer;
	uvec2 renderIndicesBuffer;
	uvec2 nodeTransformBuffer;
} PushConstants;

void main() 
{
	//TODO: CHECK FOR INVALID MATERIAL INDEX -> LOAD INVALID MATERIAL 

	MaterialBuffer materialBuffer = MaterialBuffer(PushConstants.materialBuffer);
	uint materialIndex = fs_in_index.y;

	vec4 albedo = materialBuffer.materials[materialIndex].color;
	if(materialBuffer.materials[materialIndex].albedoIndex != uint(INVALID_IMAGE_INDEX))
		albedo *= sampleTexture2D(materialBuffer.materials[materialIndex].albedoIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex);
	
	if(albedo.w < 0.1)
		discard;

	vec3 normal = vec3(normalize(fs_in_normal));
	if(materialBuffer.materials[materialIndex].normalIndex != uint(INVALID_IMAGE_INDEX))
	{
		normal = sampleTexture2D(materialBuffer.materials[materialIndex].normalIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex).xyz;
		normal = normal * 2.0 - 1.0;   
        normal = normalize(fs_in_tbn * normal); 
		normal *= (0.5 - ((fs_in_index.z >> DIRECTX_NORMALS_BIT_INDEX) & 1)) * 2.0;
	}

	float metallic = 0.0;
	if(materialBuffer.materials[materialIndex].metallicIndex != uint(INVALID_IMAGE_INDEX))
		metallic = sampleTexture2D(materialBuffer.materials[materialIndex].metallicIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex).x;

	float roughness = 0.0;
	if(materialBuffer.materials[materialIndex].roughnessIndex != uint(INVALID_IMAGE_INDEX))
		roughness = sampleTexture2D(materialBuffer.materials[materialIndex].roughnessIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex).x;
	
	fs_out_pos = vec4(fs_in_pos, 1);
	//fs_out_color = vec4(albedo.xyz, metallic);
	//fs_out_normal = vec4(normal, roughness);
	fs_out_color = vec4(albedo.xyz, 1);
	fs_out_normal = vec4(normal, 1);
	fs_out_entity = fs_in_index.x;
}