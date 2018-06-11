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

//accepts bool for if to texture on this pixel
vec4 overheadPhongDirLight(bool isTextured, vec3 colour){
	vec3 norm = normalize(normal);
	vec3 lightDir   = normalize(-overheadlight_dir.xyz);
	vec3 viewDir    = normalize(vec4(0.0, 0.0, 1.0,0) - vertex).xyz;
	vec3 halfwayDir = normalize(lightDir + viewDir);
    
    vec4 ambient = overheadlight_ambient * mtl_ambient;
	
    // The diffuse component
    float sDotN = max( dot(norm,lightDir), 0.0 );
    vec4 diffuse = overheadlight_diffuse * mtl_diffuse * sDotN;

    // The specular component
    vec4 spec = vec4(0.0);
    if ( sDotN > 0.0 )
		spec = overheadlight_specular * mtl_specular *
             pow( max( dot(norm,halfwayDir), 0.0 ), shininess );
			 
	if (isTextured){
		//diffuse = diffuse * texture(texMap, st);
	}
	
	if (isTextured){
		//spec = spec * texture(texMapSpec, st);
	}
	
    return ( ambient + diffuse + spec );
}


void main(void){
	vec4 colour;
	vec3 base = vec3(1, 1, 1);
	float r, p;

	vec3 grass = vec3(0.05, 0.4, 0.1);
	vec3 mountain = vec3(0.7, 0.7, 0.7);
	vec3 beach = vec3(0.8, 0.8, 0.5);

	vec4 diffuse = getDiffuse();
	vec4 ambient = getAmbient();
	vec4 specular = getSpec();

	//set base range
	if(vertex.y > 0.6){
		base = mountain;
	}
	else{
		base =  beach;
	}

	colour = vec4(base, 1.0f);

	//green top fade
	if(vertex.y > 0.6 && vertex.y < 0.7){
		r = vertex.y - 0.6;
		p = r*10;
		colour = vec4(base*p, 1.0f) + vec4(grass*(1-p), 1.0f);
	}
	//green solid
	if(vertex.y > 0.4 && vertex.y < 0.6){
		colour = vec4(grass, 1.0f);
	}
	//green bottom fade
	if(vertex.y > 0.3 && vertex.y < 0.4){
		r = vertex.y - 0.3;
		p = r*10;
		colour = vec4(base*(1-p), 1.0f) + vec4(grass*p, 1.0f);
	}


	

	// vec3 grass = vec3(0.3, 0.7, 0.3);
	// vec3 mountain = vec3(0.7, 0.7, 0.7);
	// vec3 beach = vec3(0.8, 0.8, 0.5);
	// vec3 colour = vec3(0, 0, 0);
	// float r;

	// if(vertPos.y > 0.57){
	// 	colour = mountain;
	// 	r = vertPos.y - 0.57;
	// 	colour = 1.7*(r*1.75 * colour);
	// }
	// if(vertPos.y < 0.4){
	// 	colour = beach;
	// 	r = vertPos.y * 2.5;
	// 	colour = 3*((1-r) * colour);
	// }
	

	// fragColour = vec4(colour+grass, 1.0);

	fragColour = ambient + diffuse * colour + specular*colour;
}
