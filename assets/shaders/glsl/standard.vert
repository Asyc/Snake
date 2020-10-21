#version 450

layout (location = 0) in vec2 pos;

layout(push_constant) uniform vertexPushConstants {
    layout(offset = 16) mat4 projectionMatrix;
    layout(offset = 80) mat4 modelMatrix;
};

void main() {
    gl_Position = projectionMatrix * modelMatrix * vec4(float(pos.x), float(pos.y), 0.0f, 1.0f);
}