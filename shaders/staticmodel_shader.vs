#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoord;
out float diffuse;
out vec4 FragPosLightSpace;
out vec3 FragPos;

uniform mat4 proj_view;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightPos;

void main()
{
    vec4 fragPos = instanceMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix)));
    vec3 Normal = normalize(normalMatrix * aNormal);
    
    vec3 lightDir = normalize(lightPos);  
    diffuse =  max(dot(Normal, lightDir), 0.0);

    gl_Position = proj_view * fragPos;
    FragPosLightSpace = lightSpaceMatrix * fragPos;
    FragPos = fragPos.xyz;
}

