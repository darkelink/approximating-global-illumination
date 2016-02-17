#version 430

layout(location = 0) in vec3 position;

layout(location = 0) out vec4 color;

uniform layout(binding = 0, r32ui) uimage3D voxels;

vec4 convRGBA8ToVec4(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U));
}

void main() {
    color = imageLoad(voxels, ivec3(position));
}
