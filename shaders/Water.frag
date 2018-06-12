#version 330

//uniform int colourMode;
in vec3 fragCol;
in vec3 normal;
in vec3 vertex;
in vec2 st;

out vec4 fragColour;

uniform bool enableTexMap;
uniform bool enableTexMapNorm;
uniform bool enableTexMapSpec;

uniform vec4 overheadlight_dir;
uniform vec4 yellowlight_pos;

uniform int lightingMode;

uniform vec4 overheadlight_ambient;     // Light ambient RGBA values
uniform vec4 overheadlight_diffuse;     // Light diffuse RGBA values  
uniform vec4 overheadlight_specular;    // Light specular RGBA values
/* 
uniform vec3 spotlight_ambient;     // Light ambient RGBA values
uniform vec3 spotlight_diffuse;     // Light diffuse RGBA values  
uniform vec3 spotlight_specular;    // Light specular RGBA values
*/
uniform vec4 mtl_ambient;  // Ambient surface colour
uniform vec4 mtl_diffuse;  // Diffuse surface colour
uniform vec4 mtl_specular; // Specular surface colour

uniform float shininess; // Specular surface colour

vec4 getDiffuse(){
	vec3 norm = normalize(normal);
	vec3 lightDir   = normalize(-overheadlight_dir.xyz);
	float sDotN = max( dot(norm,lightDir), 0.0 );
    vec4 diffuse = overheadlight_diffuse * mtl_diffuse * sDotN;
	return diffuse;
}

vec4 getAmbient(){
	vec4 ambient = overheadlight_ambient * mtl_ambient;
	return ambient;
}

vec4 getSpec(){
	vec3 norm = normalize(normal);
	vec3 lightDir   = normalize(-overheadlight_dir.xyz);
	vec3 viewDir    = normalize(-vertex);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float sDotN = max( dot(norm, halfwayDir), 0.0 );
	vec4 spec = vec4(0.0);
    if ( sDotN > 0.0 ){
		spec = overheadlight_specular * mtl_specular * pow( max( dot(norm,halfwayDir), 0.0 ), shininess );
	}

	return spec;
}


void main(void){
	vec4 colour;
	vec3 water = vec3(0.1, 0.2, 0.5);

	colour = vec4(water, 1.0);

	vec4 diffuse = getDiffuse();
	vec4 ambient = getAmbient();
	vec4 specular = getSpec();
	

	fragColour = ambient + colour * diffuse + colour * specular;
}
