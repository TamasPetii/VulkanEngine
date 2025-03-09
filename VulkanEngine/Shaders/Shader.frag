#version 460

//Input Variables
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

//Output Variables
layout(location = 0) out vec4 outColor;

//Sampler Variables
layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform PushConstants
{
    vec4 color;
} pushConstants;

void main() {
    outColor = pushConstants.color * texture(texSampler, fragTexCoord);
}