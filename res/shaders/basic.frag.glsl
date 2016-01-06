#version 330

in vec3 norm;

out vec3 color;

void main() {
    color = vec3(.1,.1,.1) + vec3(.5,.5,.5) * max(dot(norm, vec3(.5,.75,1)), 0);
}
