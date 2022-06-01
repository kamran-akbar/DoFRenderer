#version 450 core
#define renderDepth false

out vec4 FragColor;

in vec2 TexCoords;

uniform bool enableDoF;

uniform sampler2DMSArray screenTextureMS;

layout(rgba32f, binding = 3) restrict uniform image2DArray finalImage;
layout(rgba32f, binding = 4) restrict uniform image2D result;
layout(r32f, binding = 0) readonly uniform image2D depthDisc;
layout(binding = 2, r32i) restrict uniform iimage2D mergedFragCount;

float linearizeDepth(float depth)
{
	float far = 1500;
	float near = 0.1;
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    if(enableDoF){
        FragColor = imageLoad(finalImage, ivec3(gl_FragCoord.xy, 2));
        imageStore(result, ivec2(gl_FragCoord.xy), FragColor);
    }
    else{
        FragColor = vec4(0.0);
        for(int i = 0 ; i < 8; i++){
            FragColor += texelFetch(screenTextureMS, ivec3(gl_FragCoord.xy, 0), i);
        }
        FragColor /= 8;
    }
    //imageStore(finalImage, ivec2(gl_FragCoord.xy), FragColor);
    //FragColor = imageLoad(depthDisc, ivec2(gl_FragCoord.xy)).rrrr;
} 