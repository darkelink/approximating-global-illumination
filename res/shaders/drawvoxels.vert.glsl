#version 430

uniform int resolution;
uniform mat4 mvp;
uniform float scale;

flat out ivec4 position;

void main() {
    position.x = int(gl_VertexID % resolution);
    position.y = int((gl_VertexID / resolution) % resolution);
    position.z = int((gl_VertexID / (resolution * resolution)) % resolution);
    position.w = 1;

    vec4 asdf = (position - resolution/2) * scale;
    asdf.w = 1;
    gl_Position = mvp * asdf;
}
