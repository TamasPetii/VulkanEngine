#version 460

#include "Common/Images.glsl"
#include "Common/Vertex.glsl"
#include "Common/Transform.glsl"
#include "Common/Camera.glsl"
#include "Common/Material.glsl"

//Inputs
layout (location = 0) in vec3 fs_in_pos;
layout (location = 1) in vec3 fs_in_normal;
layout (location = 2) in vec2 fs_in_tex;
layout (location = 3) in flat uint fs_in_entity;
layout (location = 4) in flat uint fs_in_index;

//Outputs
layout (location = 0) out vec4 fs_out_color;
layout (location = 1) out vec4 fs_out_normal;
layout (location = 2) out uint fs_out_entity;

layout( push_constant ) uniform constants
{	
	uint entityIndex;
	uint cameraIndex;
	VertexBuffer vertexBuffer;
	CameraBuffer cameraBuffer;
	TransformBuffer transformBuffer;
	MaterialBuffer materialBuffer;
} PushConstants;


void main() 
{
	vec4 color = PushConstants.materialBuffer.materials[fs_in_index].color;

	if(PushConstants.materialBuffer.materials[fs_in_index].albedoIndex != uint(INVALID_IMAGE_INDEX))
		color *= sampleTexture2D(PushConstants.materialBuffer.materials[fs_in_index].albedoIndex, LINEAR_SAMPLER_ID, fs_in_tex);
		

	fs_out_color = color;
	fs_out_normal = vec4(normalize(fs_in_normal), 1);
	fs_out_entity = fs_in_entity;
}