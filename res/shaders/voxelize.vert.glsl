#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vertex;
out vec3 normal;

void main() {
    vertex = position;
    normal = normal;
    gl_Position = vec4(position, 1);
}
