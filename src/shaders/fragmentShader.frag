#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 worldPos;

uniform sampler2D ourTexture;
uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.0);
    if(texture(ourTexture, TexCoord).xyz != vec3(0.0))
        diffuse = light.diffuse * diff * material.diffuse * texture(ourTexture, TexCoord).xyz;
    else
        diffuse = light.diffuse * diff * material.diffuse;
    vec3 viewDir = normalize(cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor , 1.0);
}