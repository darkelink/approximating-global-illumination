#version 430 core
#extension GL_ARB_bindless_texture : require

flat in int axis;
flat in vec4 bounds;

in vec3 pos;
in vec3 norm;
in vec2 tex;

layout(pixel_center_integer) in vec4 gl_FragCoord;

layout(location = 0, bindless_sampler) uniform sampler2D Texture0;

uniform layout(binding = 0, r32ui) uimage3D voxels;

uniform int gridSize2;

vec4 convRGBA8ToVec4(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val) {
    return
        (uint(val.w) & 0x000000FF) << 24U |
        (uint(val.z) & 0x000000FF) << 16U |
        (uint(val.y) & 0x000000FF) << 8U |
        (uint(val.x) & 0x000000FF);
}

void imageAtomicRGBA8Avg(layout(r32ui) coherent  volatile  uimage3D imgUI, 
        ivec3 coords, vec4 val) {

    val.rgb *= 255.0f;
    uint newVal = convVec4ToRGBA8(val);
    uint prevStoredVal = 0;
    uint curStoredVal;

    //Loop as  long as  destination value  gets changed  by  other  threads
    while((curStoredVal = imageAtomicCompSwap(imgUI, coords, prevStoredVal, newVal))
            !=  prevStoredVal) {

        prevStoredVal = curStoredVal;
        vec4 rval = convRGBA8ToVec4(curStoredVal);
        rval.xyz = (rval.xyz * rval.w);
        // Denormalize
        vec4 curValF = rval + val;
        //Add new value
        curValF.xyz /= (curValF.w);
        // Renormalize
        newVal  = convVec4ToRGBA8(curValF);
    }
}


void main() {
    if (pos.x < bounds.x || pos.y < bounds.y || pos.x > bounds.z || pos.y > bounds.w) {
        discard;
    }

    int width = 256;

    ivec3 loc;

    if (axis == 0) { // x
        loc.x = int(gl_FragDepth);
        loc.y = int(gl_FragCoord.y);
        loc.z = int(gl_FragCoord.x);
    } else if (axis == 1) { // y
        loc.x = int(gl_FragCoord.x);
        loc.y = int(gl_FragDepth);
        loc.z = int(gl_FragCoord.y);
    } else { // z
        loc.x = int(gl_FragCoord.x);
        loc.y = int(gl_FragCoord.y);
        loc.z = int(gl_FragDepth);
    }

    imageAtomicRGBA8Avg(voxels, loc, texture(Texture0, tex));
}
