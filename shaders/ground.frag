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
uniform vec3 camPos;

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

//accepts bool for if to texture on this pixel
vec4 overheadPhongDirLight(bool isTextured, vec3 colour){
	vec3 norm = normalize(normal);
	vec3 lightDir   = normalize(-overheadlight_dir.xyz);
	vec3 viewDir    = normalize(vec3(0.0, 0.0, 1.0) - vertex);
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

float random (vec2 s) {
    return fract(sin(dot(s.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

vec2 random2(vec2 s){
	s = vec2( dot(s,vec2(127.1,311.7)), dot(s,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(s)*43758.5453123);
}

float noise (vec2 pos) {
    vec2 i = floor(pos);
    vec2 f = fract(pos);

    //four courners
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    //cubic hermine curve
    vec2 u = f*f*(3.0-2.0*f);

    // mix corners
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float splat(vec2 pos) {
	vec2 i = floor(pos);
    vec2 f = fract(pos);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

vec3 proceduralTex(float scale, int t){
	vec3 result;

	//scale of our noise
	vec2 pos = st*scale;
	float n;

	if(t == 0){
		//generate noise
		n = noise(pos);
	}
	else{
		//generate blended splater
		pos += noise2(pos*2.0);
		n = 1.0 * smoothstep(0.15,0.34,noise(pos));
    	n += smoothstep(.15,.34,noise(pos*7.));
	}

	result = vec3(n);
	return result;
}


void main(void){
	vec4 colour;
	vec3 base = vec3(1, 1, 1);
	float r, p;

	vec3 grass = vec3(0.05, 0.2, 0.05);
	vec3 mountain = vec3(0.8, 0.8, 0.8);
	vec3 beach = vec3(0.4, 0.4, 0.25);
	vec3 snow= vec3(1, 1, 1);

	//apply procedural to grass
	grass = grass + 0.35*proceduralTex(5.0, 0);

	//apply procedural to beach
	beach = beach + 0.8*proceduralTex(0.5, 0);

	//apply splatter procedural to mountain
	mountain = 0.4*vec3(mountain.x, (mountain.y - 0.4*proceduralTex(6.0, 1).y), (mountain.z - 0.7*proceduralTex(6.0, 1).z));

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

	//snow
	if(vertex.y > 0.75){
		r = vertex.y - 0.75;
		p = r*10;
		colour = colour + vec4(snow*p, 1.0f);
	}

	vec4 diffuse = getDiffuse(colour);
	vec4 ambient = getAmbient(mtl_ambient);
	vec4 specular = getSpec(mtl_specular);
	
	//fragColour += 0.5*lampLight(vec4(vertex,1.0), normalize(normal));
	fragColour = ambient + diffuse + specular;
}
