#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 view;
uniform mat4 mvp;

out vec3 norm;
out vec2 tex;

out vec3 cam_eye;
out vec3 cam_norm;

void main() {
    cam_norm = (view * vec4(normal, 0)).xyz;
    cam_eye = normalize((view * vec4(position, 1)).xyz);

    gl_Position = mvp * vec4(position, 1);

    norm = normal;
    tex = texcoord;
}
