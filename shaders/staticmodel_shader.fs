#version 300 es
precision mediump float;
in vec2 TexCoord;
in vec4 color;
in vec4 FragPosLightSpace;
in vec4 farFragPosLightSpace;
in vec3 FragPos;
in float alpha;
in float alpha2;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D depthMap;
uniform sampler2D ditherMap;
uniform vec3 backgroundColor;

uniform samplerCube skybox;
uniform vec3 viewPos;

float ambient = 0.15;
in float diffuse;
uniform vec3 lightPos;
uniform float shininess;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}                             


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
    shadow = (shadow == 0.0 ? 1.0 : max(min(shadow, 1.0),0.35));      
    FragColor = texture(texture_diffuse1, TexCoord);
    if (FragColor.a < 0.5f) {
        discard;
    }
    //vec4 dither = vec4(texture2D(ditherMap, gl_FragCoord.xy / 8.0).r / 32.0 - (1.0 / 128.0));

    //vec3 lightDir = normalize(lightPos - FragPos);  
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, Normal);  
    //float specular = pow(max(dot(viewDir, reflectDir), 0.0), 2);

    FragColor *= shadow;
    FragColor.rgb *= (diffuse + ambient); // + specular

    FragColor = mix(FragColor, vec4(backgroundColor, 1.0), alpha);
    FragColor.a = 1-alpha2;
    //FragColor.rgb += dither.rgb*0.05;

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, FragColor.rgb, 0.0f);
    FragColor.rgb =  (color != vec4(-1,-1,-1,-1) ? FragColor.rgb + fresnelSchlick(dot(Normal, viewDir), F0) * color.rgb : FragColor.rgb);

    if (color.a != -1) {
        FragColor.a = color.a;
    }
    

    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = (shininess == 1.0f ? FragColor * vec4(texture(skybox, R).rgb,1.0) : FragColor);

}
