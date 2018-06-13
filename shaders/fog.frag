#version 330

uniform vec4 camera;
uniform vec4 fogColor;
uniform mat4 projection;

in VertexData {
    vec4 mColor;
    vec4 mvertex;
} vertexin;

float getFogFactor(float d) {
    float fog_max = 20.0;
    float fog_min = 10.0;

    if (d >= fog_max) return 1;
    if (d <= fog_min) return 0;

    return 1 - (fog_max - d) / (fog_max - fog_min);
}

void main(void)
{
    vec4 V = vertexin.mvertex;
    float d = distance(camera, V);
    float alpha = getFogFactor(d);

    gl_FragColor = mix(vertexin.mColor, fogColor, alpha);
}
