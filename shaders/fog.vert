#version 330

uniform mat4 projection;

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;

out vertexdata {
    vec4 mcolor;
    vec4 mvertex;
} vertexout;

void main(void) {
    vertexout.mcolor = color;
    vertexout.mvertex = vertex;
    gl_Position = projection * vertex;
}
