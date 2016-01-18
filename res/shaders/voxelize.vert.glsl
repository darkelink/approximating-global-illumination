#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 vertPos;
out vec3 vertNorm;

void main() {
    vertPos = position;
    vertNorm = normal;
    gl_Position = vec4(position, 1);
}
