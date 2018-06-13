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

vec4 getDiffuse(vec4 c){
	vec3 norm = normalize(normal);
	vec3 lightDir   = normalize(-overheadlight_dir.xyz);
	vec3 viewDir    = normalize(-vertex);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float sDotN = max( dot(norm,halfwayDir), 0.0 );
    vec4 diffuse = overheadlight_diffuse * c * sDotN;
	return diffuse;
}

vec4 getAmbient(vec4 c){
	vec4 ambient = overheadlight_ambient * c;
	return ambient;
}

vec4 getSpec(vec4 c){
	vec3 norm = normalize(normal);
	vec3 lightDir   = normalize(-overheadlight_dir.xyz);
	vec3 viewDir    = normalize(-vertex);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float sDotN = max( dot(norm, halfwayDir), 0.0 );
	vec4 spec = vec4(0.0);
    if ( sDotN > 0.0 ){
		spec = overheadlight_specular * c * pow( max( dot(norm,halfwayDir), 0.0 ), shininess );
	}

	return spec;
}


void main(void){
	vec4 colour;
	vec3 water = vec3(0.15, 0.29, 0.44);

	colour = vec4(water, 1.0);

	vec4 diffuse = getDiffuse(colour);
	vec4 ambient = getAmbient(mtl_ambient);
	vec4 specular = getSpec(mtl_specular);

	fragColour = ambient + diffuse + specular;
}
