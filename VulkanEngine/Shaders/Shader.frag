#version 460

//Input Variables
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

//Output Variables
layout(location = 0) out vec4 outColor;

//Sampler Variables
layout(binding = 1) uniform sampler2D texSampler;

void main() {
    outColor = texture(texSampler, fragTexCoord * 2.0);
}