#extension GL_EXT_buffer_reference : require

struct Vertex {
	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec3 tangent;
	float padding;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer { 
	Vertex vertices[];
};