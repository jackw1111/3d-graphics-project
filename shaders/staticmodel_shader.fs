#version 330 core

in vec2 TexCoord;
in vec4 FragPosLightSpace;
in vec4 farFragPosLightSpace;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D depthMap;
uniform vec3 viewPos;

float ambient = 0.15;
in float diffuse;
uniform vec3 lightPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? currentDepth - closestDepth: 0.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{    
    //float dist = distance(FragPos, viewPos);
    float shadow = ShadowCalculation(FragPosLightSpace);
    shadow = (shadow == 0.0 ? 1.0 : max(min(shadow, 1.0),0.5));      
    FragColor = vec4(texture(texture_diffuse1, TexCoord).xyz,1.0);
    FragColor *= shadow;
    FragColor *= (diffuse + ambient);
}
