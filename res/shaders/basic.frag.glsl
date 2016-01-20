#version 430
#extension GL_ARB_bindless_texture : require

in vec3 norm;
in vec2 tex;

layout (location = 3, bindless_sampler) uniform sampler2D Texture0;

out vec4 color;

const vec3 lightDir = normalize(vec3(1,2,0));

void main() {
    vec4 diffuse = texture(Texture0, tex);
    color = diffuse * 0.1 + diffuse * max(dot(norm, lightDir), 0);
}
