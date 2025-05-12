#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require

struct ModelDeviceAddresses {
	uvec2 vertexBuffer;
	uvec2 materialBuffer;
	uvec2 nodeTransformBuffer;
}; 

layout(buffer_reference, std430) readonly buffer ModelDeviceAddressesBuffer { 
	ModelDeviceAddresses deviceAddresses[];
};