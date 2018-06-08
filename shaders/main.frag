#version 330
in vec4 vertex;
in vec3 normal;
uniform mat4 view;

in vec2 st;

uniform sampler2D texMap;

out vec4 fragColour;

uniform vec4 overheadlight_dir;
uniform vec4 yellowlight_pos;

uniform int lightingMode;

uniform vec3 overheadlight_ambient;     // Light ambient RGBA values
uniform vec3 overheadlight_diffuse;     // Light diffuse RGBA values  
uniform vec3 overheadlight_specular;    // Light specular RGBA values
/* 
uniform vec3 spotlight_ambient;     // Light ambient RGBA values
uniform vec3 spotlight_diffuse;     // Light diffuse RGBA values  
uniform vec3 spotlight_specular;    // Light specular RGBA values
*/
uniform vec3 mtl_ambient;  // Ambient surface colour
uniform vec3 mtl_diffuse;  // Diffuse surface colour
uniform vec3 mtl_specular; // Specular surface colour

uniform float shininess; // Specular surface colour


vec3 overheadPhongDirLight(in vec4 position, in vec3 norm){

    vec3 lightDir   = normalize(vec3(-overheadlight_dir));
	vec3 viewDir    = normalize(vec3(vec4(0.0, 0.0, 1.0,0) - position));
	vec3 halfwayDir = normalize(lightDir + viewDir);
    
    vec3 ambient = overheadlight_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(norm,lightDir), 0.0 );
    vec3 diffuse = overheadlight_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = overheadlight_specular * mtl_specular *
             pow( max( dot(norm,halfwayDir), 0.0 ), shininess );

    return ( ambient + diffuse * vec3(texture(texMap, st)) + spec );
}
/* 
vec3 headBlingPhongPointLight(in vec4 position, in vec3 norm){

    vec3 lightDir   = normalize(vec3(0,0,1) - vec3(position));
	vec3 viewDir    = normalize(vec3(vec4(0.0, 0.0, 1.0,0) - position));
	vec3 halfwayDir = normalize(lightDir + viewDir);
	
    vec3 ambient = spotlight_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(lightDir,norm), 0.0 );
    vec3 diffuse = spotlight_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec3 spec = vec3(0.0);
    if ( sDotN > 0.0 )
		spec = spotlight_specular * mtl_specular *
            pow( max( dot(norm,halfwayDir), 0.0 ), shininess );

    return ambient + diffuse * vec3(texture(texMap, st)) + spec;
} */
	

void main(void) {
		
	vec4 light = vec4(0.0);
	light = vec4(overheadPhongDirLight(vertex, normalize(normal)) , 1.0);
	//light = texture(texMap, st);
    /* if (lightingMode == 0){
		light = overheadPhongDirLight(vertex, normalize(normal));
	}
	else if (lightingMode == 1){
		light = headBlingPhongPointLight(vertex, normalize(normal));
	}
	else if (lightingMode == 2){
		light = yellowBlingPhongPointLight(vertex, normalize(normal));
	}
	else if (lightingMode == 3){
		light = vec3(texture(texMap, st));
	}
	 */
	fragColour = light;
	
	
}
