#version 430
#extension GL_ARB_bindless_texture : require

layout (local_size_x = 8, local_size_y = 8) in;

uniform layout(binding = 0, rgba8) image2D frame;
//uniform layout(location = 0, bindless_image, rgba8) image2D frame;
uniform layout(binding = 1, r32ui) uimage3D voxels;

uniform int voxelResolution;
uniform float scale;

uniform vec3 camera;
uniform vec3 topleft, topright, bottomleft, bottomright;

vec4 voxelColor(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U))/255;
}

vec4 march(vec3 origin, vec3 direction) {
    // ray = o+td

    // map origin to grid
    vec3 pos = origin / scale + voxelResolution/2;
    // TODO: handle outside grid

    vec3 move;
    move.x = direction.x > 0 ? 1 : -1;
    move.y = direction.y > 0 ? 1 : -1;
    move.z = direction.z > 0 ? 1 : -1;

    // distances to voxel edge in terms of t
    vec3 edge;

    uint color;

    int maxSamples = voxelResolution*3;

    do {
        // get new distances
        edge = abs((1 - fract(pos)) / direction);
        // check which edge the ray will cross next
        if (edge.x < edge.y && edge.x < edge.z) {
            // move ray to that edge
            pos += edge.x * direction;
            // check bounds
            if (pos.x > voxelResolution || pos.x < 0) {
                return vec4(1,0,0,1);
            }
        } else if (edge.y < edge.z) {
            pos += edge.y * direction;
            if (pos.y > voxelResolution || pos.y < 0) {
                return vec4(0,1,0,1);
            }
        } else {
            pos += edge.z * direction;
            if (pos.z > voxelResolution || pos.z < 0) {
                return vec4(0,0,1,1);
            }
        }

        color = imageLoad(voxels, ivec3(pos.xzy)).r;

        // avoid an infinate loop
        if (maxSamples-- == 0) {
            return vec4(1,1,1,1);
        }
    } while (color == 0); // assume no transparency

    return voxelColor(color);
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

    // trilinear interpolation
    vec3 direction = mix(
            mix(topleft, topright, pos.x),
            mix(bottomleft, bottomright, pos.x),
            pos.y);

    imageStore(frame, pixel, march(camera, direction));
    //imageStore(frame, pixel, vec4(direction,1));
}
