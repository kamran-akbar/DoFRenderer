#version 450 core

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

in GS_OUT
{
	vec3 ourColor;
	vec2 TexCoord;
	vec3 normal;
	vec3 worldPos;
} frag_in;

uniform sampler2DArray prevDepthmap;
uniform sampler2D depthDisc;

uniform vec2 windowDimension;
uniform vec2 cameraFarNear;
uniform sampler2D diffuseTexture;
uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

vec4 phongShading(){
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(frag_in.normal);
    vec3 lightDir = normalize(light.position - frag_in.worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.0);
    if(texture(diffuseTexture, frag_in.TexCoord).xyz != vec3(0.0))
        diffuse = light.diffuse * diff * material.diffuse * texture(diffuseTexture, frag_in.TexCoord).xyz;
    else
        diffuse = light.diffuse * diff * material.diffuse;
    vec3 viewDir = normalize(cameraPos - frag_in.worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    vec3 finalColor = ambient + diffuse + specular;
    return vec4(finalColor, 1.0);
}

float linearizeDepth(float depth) 
{
    float far = cameraFarNear.x;
    float near = cameraFarNear.y;
    float z = depth * 2.0 - 1.0;  
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
    vec2 uv = gl_FragCoord.xy / windowDimension.xy;
    float edge = texture(depthDisc, uv).r;
    if(gl_Layer >= 1){
        if(edge == 0.0){
            discard;
            return;
        }
    }
    float prevDepth = linearizeDepth(texture(prevDepthmap, vec3(uv.xy, gl_Layer - 1)).r);
    float currentDepth = linearizeDepth(gl_FragCoord.z);
    float deltaZ = 0.05;
    if(gl_Layer >= 1){
        if(currentDepth <= prevDepth + deltaZ){
           discard;
           return;
        }
    }
    gl_FragColor = phongShading();
}