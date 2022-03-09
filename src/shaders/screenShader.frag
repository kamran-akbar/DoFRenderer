#version 450 core
#define renderDepth false

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2DArray screenTexture;
uniform sampler2DArray depthTexture;
layout(rgba32f, binding = 3) restrict uniform image2D finalImage;
layout(r32f, binding = 0) readonly uniform image2D depthDisc;
layout(binding = 2, r32i) restrict uniform iimage2D mergedFragCount;

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
    //imageStore(finalImage, ivec2(gl_FragCoord.xy), FragColor);
    //FragColor = imageLoad(finalImage, ivec2(gl_FragCoord.xy));
    //FragColor = imageLoad(depthDisc, ivec2(gl_FragCoord.xy)).rrrr;
} 