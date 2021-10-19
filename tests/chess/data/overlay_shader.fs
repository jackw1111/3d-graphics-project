#version 330 core

in vec2 TexCoord;
uniform sampler2D texture1;

out vec4 FragColor;

void main() { 

	FragColor = vec4(vec3(1.0, 0.0, 0.0), 0.5);

}
