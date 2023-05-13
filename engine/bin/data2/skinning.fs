#version 300 es
precision mediump float;

in vec2 TexCoord0;
in vec3 Normal0;                                                                   
in vec3 WorldPos0;   

out vec4 FragColor;                                                              

struct VSOutput
{
    vec2 TexCoord;
    vec3 Normal;                                                                   
    vec3 WorldPos;                                                                 
};                                                                                        
                                                                                                                                     
uniform sampler2D gColorMap;                                                                                                                                                                                                                    
void main()
{                                    
    VSOutput In;
    In.TexCoord = TexCoord0;
    In.Normal   = normalize(Normal0);
    In.WorldPos = WorldPos0;                                                                       
                                                                                            
    FragColor = texture(gColorMap, In.TexCoord.xy);
}
