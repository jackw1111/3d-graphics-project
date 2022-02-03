#version 330 core

in vec3 Pos;
in vec2 TexCoord;

out vec4 FragColor;
uniform sampler2D texture1;
uniform int shaded;

void main()
{
	if (texture(texture1, TexCoord).a == 0.0f) {
		discard;
	}

	FragColor = (shaded == 1 ? texture(texture1, TexCoord) * 0.1 : texture(texture1, TexCoord));
}
