#version 430

layout(location = 0) in int resolution;

uniform mat4 mvp;

void main() {
    vec4 position;
    position.x = gl_VertexID % resolution;
    position.y = gl_VertexID / resolution % resolution;
    position.z = gl_VertexID / resolution / resolution % resolution;
    position.w = 1;
    gl_Position = mvp * position;
}
