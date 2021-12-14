#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out VS_OUT
{
	vec3 ourColor;
	vec2 TexCoord;
	vec3 normal;
	vec3 worldPos;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.worldPos = (model * vec4(aPos, 1.0f)).xyz;
    gl_Position = projection * view * vec4(vs_out.worldPos, 1.0f);
    vs_out.ourColor = aColor;
    vs_out.TexCoord = aTexCoord;
}