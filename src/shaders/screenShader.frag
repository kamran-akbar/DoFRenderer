#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2DArray screenTexture;

void main()
{
    vec3 col = texture(screenTexture, vec3(TexCoords, 3)).rgb;
    FragColor = vec4(col, 1.0);
} 