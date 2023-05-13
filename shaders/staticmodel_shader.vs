#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoord;
out float diffuse;
out vec4 FragPosLightSpace;
out vec3 FragPos;
out vec3 Normal;
out vec4 color;

uniform mat4 proj_view;
uniform vec4 colors[1000];
uniform mat4 lightSpaceMatrix;
uniform vec3 viewPos;
uniform vec3 lightPos;

uniform float nearPlane;
uniform float farPlane;

out float alpha;
out float alpha2;
 

float getFogFactor(float d, float nearPlane, float farPlane)
{
    float FogMax = 1.0f * farPlane;
    float FogMin = 0.5f * farPlane;

    if (d>=FogMax) return 1.0f;
    if (d<=FogMin) return 0.0f;

    return 1.0f - (FogMax - d) / (FogMax - FogMin);
}

float getFogFactorAlpha(float d, float nearPlane, float farPlane)
{
    float FogMax = 1.0f * farPlane;
    float FogMin = 0.7f * farPlane;

    if (d>=FogMax) return 1.0f;
    if (d<=FogMin) return 0.0f;

    return 1.0f - (FogMax - d) / (FogMax - FogMin);
}

void main()
{
    vec4 fragPos = instanceMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoords;
    
    color = colors[gl_InstanceID];
    
    mat3 normalMatrix = transpose(inverse(mat3(instanceMatrix)));
    Normal = normalize(normalMatrix * aNormal);
    
    vec3 lightDir = normalize(lightPos);  
    diffuse =  max(dot(Normal, lightDir), 0.0);

    gl_Position = proj_view * fragPos;
    FragPosLightSpace = lightSpaceMatrix * fragPos;
    FragPos = fragPos.xyz;

    float d = distance(viewPos, FragPos);
    alpha = getFogFactor(d, nearPlane, farPlane);
    alpha2 = getFogFactorAlpha(d, nearPlane, farPlane);
}

