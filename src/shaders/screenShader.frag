#version 450 core
#define renderDepth false

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D testTex;
uniform sampler2DArray screenTexture;
uniform sampler2DArray depthTexture;

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
    //int fragCount = imageLoad(mergedFragCount, ivec2(gl_FragCoord.xy / 2.0)).r;
    //FragColor = vec4(fragCount / 16.0);
    //FragColor = texture(testTex, TexCoords).rrrr;
    
} 