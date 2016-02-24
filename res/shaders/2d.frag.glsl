#version 430
#extension GL_ARB_bindless_texture : require

in vec2 tex;

out vec4 color;

layout(location = 0, bindless_sampler) uniform sampler2D Texture0;

void main() {
    color = texture(Texture0, tex);
}
