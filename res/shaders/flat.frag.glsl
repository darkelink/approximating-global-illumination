#version 430
#extension GL_ARB_bindless_texture : require

in vec3 norm;
in vec2 tex;

in vec3 cam_eye;
in vec3 cam_norm;

layout (location = 0, bindless_sampler) uniform sampler2D Texture0;

out vec4 color;

const vec3 lightDir = normalize(vec3(1,2,0));
const int specExp = 5;
const float ambient = 0.1;

void main() {
    vec4 diffuse = texture(Texture0, tex);

    vec4 spec = vec4(1,1,1,1) * 0.5
        * pow(
                max(dot(cam_eye.xyz, reflect(lightDir, cam_norm)), 0),
                specExp);

    color = spec + diffuse * ambient + diffuse * max(dot(norm, lightDir), 0);
}
