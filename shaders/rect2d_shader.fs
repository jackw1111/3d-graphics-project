#version 330 core

in vec3 Pos;
in vec2 TexCoord;
in float Alpha;

out vec4 FragColor;
uniform sampler2D texture1;

void main()
{
	if (texture(texture1, TexCoord).a == 0.0f) {
		discard;
	}

	FragColor = texture(texture1, TexCoord);
	FragColor.a *= Alpha;
}
