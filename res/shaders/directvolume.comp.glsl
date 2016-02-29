#version 430
#extension GL_ARB_bindless_texture : require

layout (local_size_x = 8, local_size_y = 8) in;

uniform layout(binding = 0, rgba8) image2D frame;
//uniform layout(location = 0, bindless_image, rgba8) image2D frame;
uniform layout(binding = 1, r32ui) uimage3D voxelColor;

uniform int voxelResolution;
uniform float scale;

uniform vec3 camera;
uniform vec3 topleft, topright, bottomleft, bottomright;

vec4 getVoxelColor(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U)) / 255;
}

vec4 march(vec3 origin, vec3 direction) {
    // ray = o+td

    // map origin to grid
    vec3 pos = -origin / scale + voxelResolution/2;
    // TODO: handle outside grid

    // use faster integar operations
    ivec3 ipos = ivec3(pos);

    ivec3 move;
    move.x = direction.x > 0 ? 1 : -1;
    move.y = direction.y > 0 ? 1 : -1;
    move.z = direction.z > 0 ? 1 : -1;

    // distances to voxel edge in terms of t (voxel width is 1)
    vec3 edge = (move+1)/2 - fract(pos);
    // one voxel distance in terms of t
    vec3 tDist = move / direction;

    uint color;

    do {
        // check which edge the ray will cross next
        if (edge.x < edge.y && edge.x < edge.z) {
            // move the ray into the next voxel
            ipos.x += move.x;
            if (ipos.x > voxelResolution || ipos.x < 0) {
                // outside the grid
                return vec4(0,0,0,1);
            }
            // ratio between other axises will increase by the equivelent of
            // 1 voxel in this direction (in terms of t)
            edge.x += tDist.x;
        } else if (edge.y < edge.z) {
            ipos.y += move.y;
            if (ipos.y > voxelResolution || ipos.y < 0) {
                return vec4(0,0,0,1);
            }
            edge.y += tDist.y;
        } else {
            ipos.z += move.z;
            if (ipos.z > voxelResolution || ipos.z < 0) {
                return vec4(0,0,0,1);
            }
            edge.z += tDist.z;
        }
        color = imageLoad(voxelColor, ipos).r;
    } while (color <= 0); // assume no transparency

    return getVoxelColor(color);
}

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

    // can only invoke 2^x number of times, reject excess
    ivec2 size = imageSize(frame);
    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }

    // convert to 0-1 range
    vec2 pos = vec2(pixel) / (vec2(size)-1);

    // bilinear interpolation
    vec3 direction = mix(
            mix(bottomleft, bottomright, pos.x),
            mix(topleft, topright, pos.x),
            pos.y);

    imageStore(frame, pixel, march(camera, direction));
}
