#version 430
#extension GL_ARB_bindless_texture : require

in vec3 norm;
in vec2 tex;

layout(location = 0, bindless_sampler) uniform sampler2D Texture0;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;

void main() {
    color = texture(Texture0, tex).xyz;
    normal = norm;
}
