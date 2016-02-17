#version 430

//layout(location = 0) in vec3 position;

layout(points) in;
layout(triangle_strip, max_vertices = 13) out;

void main() {
    gl_Position = gl_in[0].gl_Position; //+ vec4(1,1,−1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,3,−1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(3,3,−1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(3,3,−3,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(3,1,−3,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,1,−1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,1,−3,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,3,−3,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,3,−1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,3,−1,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,3,−3,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(1,1,−3,0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position; // + vec4(3,1,−3,0);
    EmitVertex();
}
// https://gist.github.com/sixman9/851154
