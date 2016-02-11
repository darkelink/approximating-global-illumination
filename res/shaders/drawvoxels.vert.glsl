#version 430

layout(location = 0) in vec3 position;

uniform mvp;

void main() {
    gl_Position = mvp * vec4(position, 1);
}
