//Author: Alex Waters
#version 330

uniform vec4 camera;
uniform vec4 fogColor;
uniform mat4 projection;

out vec4 fragColour;

in VertexData {
    vec4 mcolor;
    vec4 mvertex;
} vertexin;

float fog_factor(float d) {
    float fog_max = 20.0;
    float fog_min = 10.0;

    if (d >= fog_max) return 1;
    if (d <= fog_min) return 0;

    return 1-(fog_max-d)/(fog_max-fog_min);
}

void main(void)
{
    vec4 vertexin_mvertex = vertexin.mvertex;
    float dist = distance(camera, vertexin_mvertex);
    float alpha = fog_factor(dist);

    fragColour = mix(vertexin.mcolor, fogColor, alpha);
}
