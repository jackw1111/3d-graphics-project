#version 300 es
precision mediump float;
in vec2 TexCoords;
in vec3 Pos;

out vec4 color;
layout (location = 0) out vec3 gPosition;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    if(sampled.a < 0.8)
        discard;
	color = vec4(textColor, 1.0) * sampled;
	gAlbedoSpec = color;
	gPosition = Pos;
}
