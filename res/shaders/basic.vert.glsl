#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 MVP;

out vec3 norm;
out vec2 tex;

void main() {
    gl_Position = MVP * vec4(position, 1);
    norm = normal;
    tex = texcoord;
}
