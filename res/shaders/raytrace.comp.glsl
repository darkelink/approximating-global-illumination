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

const int MAX_DEPTH = 2;
const int MAX_SAMPLES = 10;

ivec3 lastHit;
uint lastSample;

vec3 asdf;

vec4 convVoxelData(uint val) {
    return vec4(
            float((val & 0x000000FF)),
            float((val & 0x0000FF00) >> 8U),
            float((val & 0x00FF0000) >> 16U),
            float((val & 0xFF000000) >> 24U)) / 255.0f;
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
            origin.x += move.x;
            if (lastHit.x > voxelResolution || lastHit.x < 0) {
                // outside the grid
                return false;
            }
            // ratio between other axises will increase by the equivelent of
            // 1 voxel in this direction (in terms of t)
            edge.x += tDist.x;
        } else if (edge.y < edge.z) {
            lastHit.y += move.y;
            origin.y += move.y;
            if (lastHit.y > voxelResolution || lastHit.y < 0) {
                return false;
            }
            edge.y += tDist.y;
        } else {
            lastHit.z += move.z;
            origin.z += move.z;
            if (lastHit.z > voxelResolution || lastHit.z < 0) {
                return false;
            }
            edge.z += tDist.z;
        }
        lastSample = imageLoad(voxelColor, lastHit).r;
    } while (lastSample <= 0); // assume no transparency

    asdf = origin;
    return true;
}

vec4 radience(vec3 origin, vec3 direction) {
    vec4 color = vec4(1,1,1,1);

    float r1 = noise1(0);
    float r2 = noise1(1);

    vec3 normal = lightDir;

    for (int i = 0; i < MAX_DEPTH; ++i) {
        if (!march(origin, direction)) {
            // ray missed geometry
            if (march(origin, lightDir)) {
                // last point was in shadow
                return color * 0.05;
            }

            return color * max(dot(normal, lightDir), 0);
        }

        normal = normalize(convVoxelData(imageLoad(voxelNorm, lastHit).r).xyz);
        vec3 hit = asdf + normal;
        vec4 col = convVoxelData(lastSample);


        if (!march(hit, lightDir)) {
            return color * col * max(dot(normal, lightDir), 0.0);
        }

        vec3 u = normalize(cross((abs(normal.x) > .1 ? vec3(0, 1, 0) : vec3(1, 0, 0)), normal));
        vec3 v = cross(normal,u);

        r1 = noise1(r1);
        r2 = noise1(r2);
        float r2s = sqrt(r2);

        direction = normalize(u*cos(r1)*r2s + v*sin(r1)*r2s + normal*sqrt(1 - r2));
        origin = hit;

        color *= col * max(dot(normal, direction), 0.1);
    }

    return color * max(dot(normal, lightDir), 0);
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

    vec4 color = vec4(0,0,0,0);

    vec4 diffuse;
    vec3 normal;
    vec3 intercept;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        color += radience(origin, direction) * 1/MAX_SAMPLES;
    }

    imageStore(frame, pixel, color);
}
