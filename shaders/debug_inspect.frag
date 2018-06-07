#version 330

//uniform int colourMode;

in vec3 fragCol;
in vec3 normal;

out vec4 fragColour;

void main(void){
	float k = fragCol.z;
	fragColour = vec4(fragCol.x , 0.1 , normal.z, k+0.9);
	/* if (colourMode == 1){
		fragColour = vec4(normal.x , normal.y , normal.z , 1.0);
	}
	else if (colourMode == 2){
		fragColour = vec4(fragCol.x , fragCol.y , fragCol.z , 1.0);
	}
	else{
		fragColour = vec4(fragCol.x , fragCol.y , fragCol.z , 1.0);
	} */
    //fragColour = vec4(1.0 , 1.0 , 1.0 , 1.0);
}
