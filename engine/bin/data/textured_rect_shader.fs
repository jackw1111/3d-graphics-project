#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
   if (texture(texture1, TexCoord).a == 0.0f) {
   	discard;
   }
	FragColor = texture(texture1, TexCoord);
}