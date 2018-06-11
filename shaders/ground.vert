#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texture;

uniform mat4 projection;
uniform mat4 modelviewMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;
uniform vec3 Kd;

out vec3 fragCol;
out vec3 normal;
out vec3 vertex;
out vec2 st;

void main(void) {

	vec4 objPos = viewMatrix * modelviewMatrix * vec4(a_vertex, 1.0);
	gl_Position = projection * objPos;
	
	fragCol = Kd;
	
    normal = normalize(normalMatrix * a_normal);
	st = mat2(vec2(1.0 , 0.0) , vec2(0.0, -1.0)) * a_texture;

	vertex = a_vertex;
	
}