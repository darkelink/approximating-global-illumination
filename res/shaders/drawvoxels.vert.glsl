#version 430

uniform int resolution;
uniform mat4 mvp;

flat out ivec4 position;

void main() {
    ivec4 position;
    position.x = gl_VertexID % resolution;
    position.y = (gl_VertexID / resolution) % resolution;
    position.z = (gl_VertexID / (resolution * resolution)) % resolution;
    position.w = 1;
    gl_Position = mvp * position;
}
