// Jordan Hoskin-Stark
#version 330
layout (location = 0) in vec3 a_vertex;

out vec3 st;

uniform mat4 projection;
uniform mat4 viewMatrix;

void main(){

    st = a_vertex;
    gl_Position = projection * viewMatrix * vec4(a_vertex, 1.0);
	
}  
