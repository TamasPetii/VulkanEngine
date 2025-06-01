#version 460

#include "Common/Index.glsl"
#include "Common/Camera.glsl"
#include "Common/Vertex.glsl"
#include "Common/SimpleTransform.glsl"

layout(location = 0) out flat uint vs_out_id;

layout( push_constant ) uniform constants
{	
	uvec2 cameraBuffer;
	uvec2 pointLightBuffer;
	uvec2 transformBufferAddress;
	uvec2 vertexBufferAddress;
	uvec2 indexBufferAddress;
	vec2 viewPortSize;
	vec3 padding;
	uint cameraIndex;
} PushConstants;

void main() 
{
	uint lightIndex = gl_InstanceIndex; //TODO: READ THIS FROM INSTANCE INDEX MAP!

	uint vertexIndex = IndexBuffer(PushConstants.indexBufferAddress).indices[gl_VertexIndex];
	Vertex v = VertexBuffer(PushConstants.vertexBufferAddress).vertices[vertexIndex];

	vec4 worldPosition = SimpleTransformBuffer(PushConstants.transformBufferAddress).transforms[lightIndex] * vec4(v.position, 1.0);
	gl_Position = CameraBuffer(PushConstants.cameraBuffer).cameras[PushConstants.cameraIndex].viewProj * worldPosition;

	vs_out_id = lightIndex;
}