#version 330 core
//layout(early_fragment_tests) in;
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main() {
	vec4 tex = texture(texture1, TexCoord);
	if ( tex.a < 0.1) {
		discard;
	}
	FragColor = tex;
	FragColor.a = 0.5f;
}
