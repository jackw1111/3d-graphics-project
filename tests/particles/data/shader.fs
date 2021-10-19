#version 330 core
in vec2 TexCoord;
uniform sampler2D texture1;
out vec4 FragColor;
void main() { 
vec4 red = vec4(1,0,0,1);
FragColor = texture(sampler ,tex coords)+red;

}