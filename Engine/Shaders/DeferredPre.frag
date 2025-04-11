#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/Images.glsl"
#include "Common/Material.glsl"
#include "Common/Incides.glsl"

//Inputs
layout (location = 0) in vec3 fs_in_pos;
layout (location = 1) in vec3 fs_in_normal;
layout (location = 2) in vec2 fs_in_tex;
layout (location = 3) out flat uvec2 fs_in_index;

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
	uvec2 materialIndexBuffer;
} PushConstants;

void main() 
{
	uint materialIndex = PushConstants.renderMode == NORMAL_INSTANCED ? fs_in_index.y : MaterialIndexBuffer(PushConstants.materialIndexBuffer).materialIndices[fs_in_index.y];

	vec4 albedo = MaterialBuffer(PushConstants.materialBuffer).materials[materialIndex].color;
	if(MaterialBuffer(PushConstants.materialBuffer).materials[materialIndex].albedoIndex != uint(INVALID_IMAGE_INDEX))
		albedo *= sampleTexture2D(MaterialBuffer(PushConstants.materialBuffer).materials[materialIndex].albedoIndex, LINEAR_ANISOTROPY_SAMPLER_ID, fs_in_tex);
		
	vec3 normal = vec3(normalize(fs_in_normal));
	if(MaterialBuffer(PushConstants.materialBuffer).materials[materialIndex].normalIndex != uint(INVALID_IMAGE_INDEX))
		normal = sampleTexture2D(MaterialBuffer(PushConstants.materialBuffer).materials[materialIndex].normalIndex, NEAREST_SAMPLER_ID, fs_in_tex).xyz;

	fs_out_color = albedo;
	fs_out_normal = vec4(normal, 1);
	fs_out_entity = fs_in_index.x;
}