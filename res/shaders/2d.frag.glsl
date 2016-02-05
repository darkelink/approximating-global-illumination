#version 430
#extension GL_ARB_bindless_texture : require

in vec2 tex;

out vec4 color;

layout(location = 0, bindless_sampler) uniform sampler2D Texture0;
layout(location = 1, bindless_sampler) uniform sampler2D Texture1;

const vec4 lightDir = normalize(vec4(1,2,0,1));

void main() {
    vec4 diffuse = texture(Texture0, tex);
    color = diffuse * dot(texture(Texture1, tex), lightDir);
}
