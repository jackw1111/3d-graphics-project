#version 330 core

in vec2 TexCoord;
in float diffuse;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform int shadowPass;  

float ambient = 0.15;                                        
                                                                                                        
void main()
{    
    FragColor = (shadowPass == 1 ? vec4(0,0,0,1) : vec4(texture(texture_diffuse1, TexCoord).xyz * (diffuse + ambient), 1.0));
}


