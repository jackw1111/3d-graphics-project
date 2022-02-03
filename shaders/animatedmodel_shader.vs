#version 330 core
#extension GL_ARB_arrays_of_arrays : enable 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;
layout (location = 7) in mat4 instanceMatrix;

out vec2 TexCoord;
out float diffuse;
out vec3 color;
out vec3 FragPos;
out vec3 normal;


uniform mat4 proj_view;
uniform mat4 gBones[20][50];
uniform vec3 colors[20];
uniform mat4 lightSpaceMatrix;
uniform int shadowPass;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float nearPlane;
uniform float farPlane;

out float alpha;
out float alpha2;


/*float getFogFactor(float d, float nearPlane, float farPlane)
{
    float FogMax = 1.0f * farPlane;
    float FogMin = 0.5f * farPlane;

    if (d>=FogMax) return 1.0f;
    if (d<=FogMin) return 0.0f;

    return farPlane;//1.0f - (FogMax - d) / (FogMax - FogMin);
}

float getFogFactorAlpha(float d, float nearPlane, float farPlane)
{
    float FogMax = 1.0f * farPlane;
    float FogMin = 0.7f * farPlane;

    if (d>=FogMax) return 1.0f;
    if (d<=FogMin) return 0.0f;

    return 1.0f - (FogMax - d) / (FogMax - FogMin);
}*/

void main()
{   
    mat4 BoneTransform = gBones[gl_InstanceID][BoneIDs[0]] * Weights[0];
    BoneTransform     += gBones[gl_InstanceID][BoneIDs[1]] * Weights[1];
    BoneTransform     += gBones[gl_InstanceID][BoneIDs[2]] * Weights[2];
    BoneTransform     += gBones[gl_InstanceID][BoneIDs[3]] * Weights[3];

    vec4 fragPos = instanceMatrix * BoneTransform * vec4(aPos, 1.0);

    if (shadowPass == 1) {
        gl_Position = lightSpaceMatrix * fragPos;
        return;
    }
    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix * BoneTransform)));
    vec3 Normal = normalize(normalMatrix * aNormal);

    color = colors[gl_InstanceID];

    gl_Position = proj_view * fragPos;
    TexCoord = aTexCoord;    
    vec3 lightDir = normalize(lightPos);  
    diffuse =  max(dot(Normal, lightDir), 0.0);

    normal = Normal;

    //float d = distance(viewPos, FragPos);
    //alpha = getFogFactor(d, nearPlane, farPlane);
    //alpha2 = getFogFactorAlpha(d, nearPlane, farPlane);
    FragPos = fragPos.xyz;

}
