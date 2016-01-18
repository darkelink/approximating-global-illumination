#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 vertPos[];
in vec3 vertNorm[];

out vec3 pos;
out vec3 norm;

flat out int axis; // x=0, y=1, z=2

uniform int gridSize;

const mat4 xprojection = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);
const mat4 yprojection = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);
const mat4 zprojection = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0);

void main() {

    // find maximum projection axis
    mat4 projection;

    vec3 faceNormal = normalize(cross(vertPos[1]-vertPos[0], vertPos[2]-vertPos[0]));
    float sizex = abs(faceNormal.x);
    float sizey = abs(faceNormal.x);
    float sizez = abs(faceNormal.x);

    if (sizex > sizey && sizex > sizez) {
        projection = xprojection;
        axis = 0;
    } else if (sizey > sizez) {
        projection = yprojection;
        axis = 1;
    } else {
        projection = zprojection;
        axis = 2;
    }

    vec4 vert[3];
    vert[0] = projection * gl_in[0].gl_Position;
    vert[1] = projection * gl_in[1].gl_Position;
    vert[2] = projection * gl_in[2].gl_Position;

    gl_Position = vert[0];
    pos = vert[0].xyz;
    norm = vertNorm[0];
    EmitVertex();

    gl_Position = vert[1];
    pos = vert[1].xyz;
    norm = vertNorm[1];
    EmitVertex();

    gl_Position = vert[2];
    pos = vert[2].xyz;
    norm = vertNorm[2];
    EmitVertex();

    EndPrimitive();
}
