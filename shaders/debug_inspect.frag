#version 330

//uniform int colourMode;

in vec3 fragCol;
in vec3 normal;

out vec4 fragColour;

void main(void){

	fragColour = vec4(normal.xyz , 1.0);
}
