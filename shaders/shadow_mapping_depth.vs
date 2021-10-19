#version 330 core
#extension GL_ARB_arrays_of_arrays : enable 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;
layout (location = 7) in mat4 instanceMatrix;

const int MAX_BONES = 50;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out vec3 color;

out vec2 TexCoord0;
out vec3 Normal0;                                                                   
out vec3 WorldPos0;  

out float diffuse;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 MVP;
uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProjection;
uniform mat4 gBones[20][MAX_BONES];
uniform vec3 colors[20];

uniform bool invertedNormals;
uniform int modelID;


uniform mat4 lightSpaceMatrix;


struct Light {
    vec3 position;
    vec3 color;
};


uniform int no_of_lights; 
const int MAX_LIGHTS = 32;
uniform Light lights[MAX_LIGHTS];

void main()
{   
    mat4 BoneTransform = gBones[gl_InstanceID][BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[gl_InstanceID][BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[gl_InstanceID][BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[gl_InstanceID][BoneIDs[3]] * Weights[3];

    vec4 fragPos = instanceMatrix * BoneTransform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix * BoneTransform)));
    Normal = normalize(normalMatrix * (invertedNormals ? -aNormal : aNormal));

    vec3 lightDir = normalize(lights[0].position - fragPos.xyz);  
    diffuse =  max(dot(Normal, lightDir), 0.0);
    color = colors[gl_InstanceID];
    if (color != vec3(-1,-1,-1)) {
        fragPos = vec4(fragPos.xyz + Normal*0.05, 1.0);
    }
    FragPos = fragPos.xyz;
    gl_Position = lightSpaceMatrix * fragPos;

}
