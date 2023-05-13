#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aAlpha;
out vec3 Pos;
out vec2 TexCoord;
out float Alpha;

uniform mat4 viewProjection;

void main()
{
	gl_Position = viewProjection * vec4(aPos, 1.0);
	Pos = aPos;
	TexCoord = aTexCoord;
	Alpha = aAlpha;
}
