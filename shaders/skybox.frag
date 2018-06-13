// Jordan Hoskin-Stark
#version 330
out vec4 fragColour;

in vec3 st;

uniform samplerCube skybox;

void main(){
    
    fragColour = texture(skybox, st);

}
