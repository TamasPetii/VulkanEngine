#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require
#extension GL_EXT_nonuniform_qualifier : require

struct DirectionLight {
	vec3 color;
	float strength;
	vec3 direction;
	float shininess;
}; 

layout(buffer_reference, std430) readonly buffer DirectionLightBuffer { 
	DirectionLight lights[];
};

//layout(set = 1, binding = 0) uniform texture2D u_dirLightShadowTextures[];