#version 330

//uniform int colourMode;

in vec3 fragCol;
in vec3 normal;
in vec3 vertPos;

out vec4 fragColour;

void main(void){

	vec3 grass = vec3(0.3, 0.7, 0.3);
	vec3 mountain = vec3(0.7, 0.7, 0.7);
	vec3 beach = vec3(0.8, 0.8, 0.5);
	vec3 colour = vec3(0, 0, 0);
	float r;

	if(vertPos.y > 0.57){
		colour = mountain;
		r = vertPos.y - 0.57;
		colour = 1.7*(r*1.75 * colour);
	}
	if(vertPos.y < 0.4){
		colour = beach;
		r = vertPos.y * 2.5;
		colour = 3*((1-r) * colour);
	}
	

	fragColour = vec4(colour+grass, 1.0);
}
