#version 430
#extension GL_ARB_bindless_texture : require

layout (local_size_x = 8, local_size_y = 8) in;

uniform layout(binding = 0, rgba8) image2D frame;
//uniform layout(location = 0, bindless_image, rgba8) image2D frame;
uniform layout(binding = 1, r32ui) uimage3D voxelColor;
uniform layout(binding = 2, r32ui) uimage3D voxelNorm;

uniform int voxelResolution;
uniform float scale;

uniform vec3 camera;
uniform vec3 topleft, topright, bottomleft, bottomright;

const vec3 lightDir = normalize(vec3(1,1.5,0.2));
const float ambient = 0.1;

ivec3 lastHit;
uint lastSample;

vec4 convVoxelData(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U)) / 255;
}

bool march(vec3 origin, vec3 direction) {
    // ray = o+td

    // use faster integar operations
    lastHit = ivec3(origin);

    ivec3 move;
    move.x = direction.x > 0 ? 1 : -1;
    move.y = direction.y > 0 ? 1 : -1;
    move.z = direction.z > 0 ? 1 : -1;

    // distances to voxel edge in terms of t (voxel width is 1)
    vec3 edge = (move+1)/2 - fract(origin);
    // one voxel distance in terms of t
    vec3 tDist = move / direction;

    do {
        // check which edge the ray will cross next
        if (edge.x < edge.y && edge.x < edge.z) {
            // move the ray into the next voxel
            lastHit.x += move.x;
            if (lastHit.x > voxelResolution || lastHit.x < 0) {
                // outside the grid
                return false;
            }
            // ratio between other axises will increase by the equivelent of
            // 1 voxel in this direction (in terms of t)
            edge.x += tDist.x;
        } else if (edge.y < edge.z) {
            lastHit.y += move.y;
            if (lastHit.y > voxelResolution || lastHit.y < 0) {
                return false;
            }
            edge.y += tDist.y;
        } else {
            lastHit.z += move.z;
            if (lastHit.z > voxelResolution || lastHit.z < 0) {
                return false;
            }
            edge.z += tDist.z;
        }
        lastSample = imageLoad(voxelColor, lastHit).r;
    } while (lastSample <= 0); // assume no transparency

    return true;
}

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

    // can only invoke 2^x number of times, reject excess
    ivec2 size = imageSize(frame);
    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }

    // convert to 0-1 range
    vec2 pos = vec2(pixel) / (size-1);

    // bilinear interpolation
    vec3 direction = mix(
            mix(bottomleft, bottomright, pos.x),
            mix(topleft, topright, pos.x),
            pos.y);

    // map origin to grid
    vec3 origin = -camera / scale + voxelResolution/2;
    // TODO: handle outside grid

    if (march(origin, direction)) {
        vec4 diffuse = convVoxelData(lastSample);
        vec3 normal = convVoxelData(imageLoad(voxelNorm, lastHit).r).xyz;
        vec4 color;

        if (march(vec3(lastHit + vec3(0,1,0)), lightDir)) {
            // in shadow
            color = diffuse * 0.1;
        } else {
            color = diffuse * max(dot(normal, lightDir), 0);
        }
        imageStore(frame, pixel, color);

    } else {
        // ray didn't hit anything
        imageStore(frame, pixel, vec4(1,1,1,1));
    }
}
