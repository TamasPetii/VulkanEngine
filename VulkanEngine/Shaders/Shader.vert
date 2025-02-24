#version 460

//Input Variables
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

//Output Variables
layout(location = 0) out vec3 fragColor;

//Unifrom Buffers
layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}