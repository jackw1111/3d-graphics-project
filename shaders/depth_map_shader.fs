#version 330 core
out vec4 FragColor;
//in vec3 TexCoords;

//uniform sampler2D depthMap;

void main()
{    
    //FragColor = texture(depthMap, TexCoords);
    FragColor = vec4(1,0,1,1);
}