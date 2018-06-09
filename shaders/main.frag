#version 330
in vec4 vertex;
in vec3 normal;
uniform mat4 view;

in vec2 st;

uniform sampler2D texMap;
uniform sampler2D texMapNormal;
uniform sampler2D texMapSpec;

uniform bool enableTexMap;
uniform bool enableTexMapNorm;
uniform bool enableTexMapSpec;

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
			 
	if (enableTexMap){
		diffuse = diffuse * vec3(texture(texMap, st));
	}
	
	if (enableTexMapSpec){
		spec = spec * vec3(texture(texMapSpec, st));
	}
	
    return ( ambient + diffuse + spec );
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

mat3 calcTBN(){
	// derivations of the fragment position
	vec3 pos_dx = dFdx( vec3(vertex) );
	vec3 pos_dy = dFdy( vec3(vertex) );
	// derivations of the texture coordinate
	vec2 texC_dx = dFdx( st );
	vec2 texC_dy = dFdy( st );
	// tangent vector and binormal vector
	vec3 t = texC_dy.y * pos_dx - texC_dx.y * pos_dy;
	vec3 b = texC_dx.x * pos_dy - texC_dy.x * pos_dx;

	t = t - normal * dot( t, normal ); // orthonormalization ot the tangent vectors
	b = b - normal * dot( b, normal ); // orthonormalization of the binormal vectors to the normal vector 
	b = b - t * dot( b, t ); // orthonormalization of the binormal vectors to the tangent vector
	mat3 tbn = mat3( normalize(t), normalize(b), normal );
	
	return tbn;
	
}	

void main(void) {


	
	vec4 light = vec4(0.0);
	vec3 N = normal;
	
	if (enableTexMapNorm){
		mat3 tbn = calcTBN();
		vec3 NN = texture(texMapNormal, st).xyz; // normal map
		N =  tbn*normalize(2.0*NN.xyz-1.0);
		//N =  normalize(2.0*NN.xyz-1.0);
    }
	
	light.xyz = overheadPhongDirLight(vertex, normalize(N));
	light = vec4(light.xyz,1.0);
	
	fragColour = light; //texture(texMap, st);
	
	
}
