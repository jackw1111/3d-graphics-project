#version 300 es
precision mediump float;
in vec2 TexCoord;
in float diffuse;
in vec4 color;
in float alpha;
in float alpha2;
in vec3 FragPos;
in vec3 normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D ditherMap;
uniform int shadowPass;  
uniform vec3 backgroundColor;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform float shininess;
uniform samplerCube skybox;


float ambient = 0.15;           

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}                             
                      
void main()
{    
	vec4 tex = texture(texture_diffuse1, TexCoord);
    FragColor = (shadowPass == 1 ? vec4(0,0,0,1) :  vec4(tex.rgb * (diffuse + ambient), tex.a));


    vec4 dither = vec4(texture2D(ditherMap, gl_FragCoord.xy / 8.0).r / 32.0 - (1.0 / 128.0));


    FragColor = mix(FragColor, vec4(backgroundColor, 1.0), alpha);
    FragColor.a = 1-alpha2;
    //FragColor.rgb += dither.rgb*0.05;

    vec3 F0 = vec3(0.04);
	F0      = mix(F0, FragColor.rgb, 0.0f);

	vec3 lightDir   = normalize(lightPos - FragPos);
	vec3 viewDir    = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

    FragColor.rgb =  (color != vec4(-1,-1,-1,-1) ? FragColor.rgb + fresnelSchlick(dot(normal, viewDir), F0) * color.rgb : FragColor.rgb);

    if (color.a != -1) {
        FragColor.a = color.a;
    }

    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(normal));
    FragColor = (shininess == 1.0f ? FragColor * vec4(texture(skybox, R).rgb,1.0) : FragColor);
}


