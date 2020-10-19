#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fs_Color;

layout(push_constant) uniform fragmentPushConstants {
    layout(offset = 0) vec4 color;
};

void main() {
    fs_Color = color;
}