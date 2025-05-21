#version 460

#include "Common/Plane.glsl"

layout(location = 0) out vec2 vs_out_tex;
layout(location = 1) out flat uint vs_out_id;

void main() 
{
	gl_Position = positions[gl_VertexIndex];
	vs_out_tex	= texCoords[gl_VertexIndex];
	vs_out_id = gl_InstanceIndex;
}