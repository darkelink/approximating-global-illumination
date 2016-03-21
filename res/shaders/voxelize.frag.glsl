#version 430 core
#extension GL_ARB_bindless_texture : require

flat in int axis;

in vec3 norm;
in vec2 tex;

layout(pixel_center_integer) in vec4 gl_FragCoord;

layout(location = 0, bindless_sampler) uniform sampler2D Texture0;

uniform layout(binding = 0, r32ui) uimage3D voxelColor;
uniform layout(binding = 1, r32ui) uimage3D voxelNorm;

uniform int gridSize;

vec4 getVoxelData(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U));
}

uint calcVoxelData(vec4 val) {
    return
        (uint(val.w) & 0x000000FF) << 24U |
        (uint(val.z) & 0x000000FF) << 16U |
        (uint(val.y) & 0x000000FF) << 8U |
        (uint(val.x) & 0x000000FF);
}

void storeVoxelData(layout(r32ui) coherent  volatile  uimage3D img, ivec3 coords,
        vec4 data) {

    data.rgb *= 255.0f;
    uint newVal = calcVoxelData(data);
    uint prev = 0;
    uint current;

    // spinlock until image write succeeds
    while((current = imageAtomicCompSwap(img, coords, prev, newVal))
            !=  prev) {

        prev = current;
        vec4 rval = getVoxelData(current);

        rval.xyz = rval.xyz * rval.w;
        rval.xyz += data.xyz;
        // use alpha channel for accumulator
        rval.w += 1;
        rval.xyz /= rval.w;

        newVal = calcVoxelData(rval);

    }
}


void main() {
    ivec3 loc;

    if (axis == 0) { // x
        loc.x = int(gl_FragCoord.z * gridSize + 1);
        loc.y = int(gl_FragCoord.y);
        loc.z = int(gl_FragCoord.x);
    } else if (axis == 1) { // y
        loc.x = int(gridSize - gl_FragCoord.y);
        loc.y = int(gl_FragCoord.z * gridSize);
        loc.z = int(gl_FragCoord.x);
    } else { // z
        loc.x = int(gridSize - gl_FragCoord.x);
        loc.y = int(gl_FragCoord.y);
        loc.z = int(gl_FragCoord.z * gridSize);
    }

    storeVoxelData(voxelColor, loc, texture(Texture0, tex));
    storeVoxelData(voxelNorm, loc, vec4(norm, 1));
}
