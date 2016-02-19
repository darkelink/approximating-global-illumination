#version 430

uniform layout(binding = 0, rgba32f) image2D frame;
uniform layout(binding = 0, r32ui) uimage3D voxels;

uniform float voxelWidth;
uniform int voxelResolution;
uniform float scale;

vec4 voxelColor(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U))/255;
}

vec4 march(vec3 origin, vec3 direction) {
    // map origin to grid
    vec3 pos = origin / scale + voxelResolution/2;
    // TODO: handle outside grid

    // distances to voxel edge in terms of t
    vec3 edge = pos % 1;
    // one full voxel distance in terms of t
    vec3 tDist = 1 / direction;

    ivec3 move;
    move.x = direction.x > 0 ? 1 : -1;
    move.y = direction.y > 0 ? 1 : -1;
    move.z = direction.z > 0 ? 1 : -1;

    uint color;

    do {
        if (edge.x < edge.y && edge.x < edge.z) {
            pos.x += move.x;
            if (o.x > voxelResolution) {
                return vec4(0,0,0,0);
            }
            edge.x += tDist.x;
        } else if (edge.y < edge.z) {
            pos.y += move.y;
            if (pos.y > voxelResolution) {
                return vec4(0,0,0,0);
            }
            edge.y += tDist.y;
        } else {
            pos.z += move.z;
            if (pos.z > voxelResolution) {
                return vec4(0,0,0,0);
            }
            edge.z += tDist.z;
        }
        color = imageLoad(voxels, ivec3(pos)).r;
    } while (color <= 0);

    return voxelColor(color);
}

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

    // can only invoke 2^x number of times, reject excess
    ivec2 size = imageSize(frame);
    if (pixel.x > size.x || pixel.y > size.y) {
        return;
    }

    // convert to 0-1 range
    vec2 pos = vec2(pixel) / vec2(size);

    vec2 direction = normalize(vec2(0));

    imageStore(frame, pixel, march(camera, direction));
}
