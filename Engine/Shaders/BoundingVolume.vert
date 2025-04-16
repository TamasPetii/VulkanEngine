#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/Vertex.glsl"
#include "Common/Camera.glsl"
#include "Common/BoundingVolume.glsl"

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBuffer;
	uvec2 vertexBuffer;
	uvec2 transformBuffer;
	vec4 color;
} PushConstants;

void main() 
{
	Vertex v = VertexBuffer(PushConstants.vertexBuffer).vertices[gl_VertexIndex];
	gl_Position = CameraBuffer(PushConstants.cameraBuffer).cameras[PushConstants.cameraIndex].viewProj * BoundingVolumeBuffer(PushConstants.transformBuffer).transforms[gl_InstanceIndex] * vec4(v.position + v.normal * 0.001, 1.0);
}