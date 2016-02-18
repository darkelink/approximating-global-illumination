#version 430

flat in ivec4 position;

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
    vec4 c = imageLoad(voxels, position.xyz);
    if (c.a == 0) {
        discard;
    }
    //color = c;
    color = vec4(1,1,1,1);
}
