#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require

layout(buffer_reference, std430) readonly buffer AnimationVertexBoneDeviceAddressesBuffer { 
	uvec2 deviceAddresses[];
};

layout(buffer_reference, std430) readonly buffer AnimationNodeTransformDeviceAddressesBuffer { 
	uvec2 deviceAddresses[];
};