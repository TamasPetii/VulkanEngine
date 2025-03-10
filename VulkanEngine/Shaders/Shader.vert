#version 460

//Input Variables
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

//Output Variables
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragCoord;

//Unifrom Buffers
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragCoord = inTexCoord;
}