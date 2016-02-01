#version 430 core

flat in int axis;

in vec3 pos;
in vec3 norm;
in vec2 tex;

layout(location = 0) out vec4 gl_FragColor;
layout(pixel_center_integer) in vec4 gl_FragCoord;

uniform int gridSize;

void main() {
    //uint level = atomicCounterIncrement(voxelFragments);
}
