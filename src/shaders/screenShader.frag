#version 450 core
#define renderDepth false

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D testTex;
uniform sampler2DArray screenTexture;
uniform sampler2DArray depthTexture;
layout(rgba32f, binding = 3) restrict uniform image2D finalImage;

void main()
{
    if(!renderDepth) {
        vec3 col = texture(screenTexture, vec3(TexCoords, 0)).rgb;
        FragColor = vec4(col, 1.0);
    }
    else {
        float depth = texture(depthTexture, vec3(TexCoords, 0)).r;
        FragColor = vec4(depth);
    }
    FragColor = imageLoad(finalImage, ivec2(gl_FragCoord.xy));
    //FragColor = texture(testTex, TexCoords).rrrr;
} 