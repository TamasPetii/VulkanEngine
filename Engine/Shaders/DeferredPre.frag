#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/RenderDefines.glsl"
#include "Common/Images.glsl"
#include "Common/Material.glsl"

//Inputs
layout (location = 0) in vec3 fs_in_pos;
layout (location = 1) in vec3 fs_in_normal;
layout (location = 2) in vec2 fs_in_tex;
layout (location = 3) in flat uvec2 fs_in_index;
layout (location = 4) in mat3 fs_in_tbn;

//Outputs
layout (location = 0) out vec4 fs_out_color;
layout (location = 1) out vec4 fs_out_normal;
layout (location = 2) out uint fs_out_entity;

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
} PushConstants;

void main() 
{
	//TODO: CHECK FOR INVALID MATERIAL INDEX -> LOAD INVALID MATERIAL 

	MaterialBuffer materialBuffer = MaterialBuffer(PushConstants.materialBuffer);
	uint materialIndex = fs_in_index.y;

	vec4 albedo = materialBuffer.materials[materialIndex].color;
	if(materialBuffer.materials[materialIndex].albedoIndex != uint(INVALID_IMAGE_INDEX))
		albedo *= sampleTexture2D(materialBuffer.materials[materialIndex].albedoIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex);
		
	vec3 normal = vec3(normalize(fs_in_normal));
	if(materialBuffer.materials[materialIndex].normalIndex != uint(INVALID_IMAGE_INDEX))
	{
		normal = sampleTexture2D(materialBuffer.materials[materialIndex].normalIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex).xyz;
        normal = normal * 2.0 - 1.0;   
        normal = normalize(fs_in_tbn * normal); 
	}

	fs_out_color = albedo;
	fs_out_normal = vec4(normal, 1);
	fs_out_entity = fs_in_index.x;
}