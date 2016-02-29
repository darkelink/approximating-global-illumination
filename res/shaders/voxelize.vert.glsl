#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

out vec3 vertPos;
out vec3 vertNorm;
out vec2 texCoord;

void main() {
    vertPos = position;
    vertNorm = normal;
    texCoord = texcoord;
    gl_Position = vec4(position, 1);
}
