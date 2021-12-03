#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 normal;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    normal = mat3(transpose(inverse(model))) * aNormal;
    worldPos = (model * vec4(aPos, 1.0f)).xyz;
    gl_Position = projection * view * vec4(worldPos, 1.0f);
    ourColor = aColor;
    TexCoord = aTexCoord;
}