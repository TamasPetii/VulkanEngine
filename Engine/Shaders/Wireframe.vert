#version 460
#extension GL_EXT_buffer_reference_uvec2 : require

#include "Common/Index.glsl"
#include "Common/Vertex.glsl"
#include "Common/Camera.glsl"
#include "Common/SimpleTransform.glsl"

layout( push_constant ) uniform constants
{	
	uint renderMode;
	uint cameraIndex;
	uvec2 cameraBufferAddress;
	uvec2 transformBufferAddress;
	uvec2 vertexBufferAddress;
	uvec2 indexBufferAddress;
	uvec2 padding;
	vec4 color;
} PushConstants;

void main() 
{
	uint vertexIndex = IndexBuffer(PushConstants.indexBufferAddress).indices[gl_VertexIndex];
	Vertex v = VertexBuffer(PushConstants.vertexBufferAddress).vertices[vertexIndex];
	gl_Position = CameraBuffer(PushConstants.cameraBufferAddress).cameras[PushConstants.cameraIndex].viewProj * SimpleTransformBuffer(PushConstants.transformBufferAddress).transforms[gl_InstanceIndex] * vec4(v.position + v.normal * 0.001, 1.0);
}