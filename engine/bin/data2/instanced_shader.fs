#version 300 es
precision mediump float;
//layout(early_fragment_tests) in;
out vec4 FragColor;

in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main() {
	if (texture(texture1, TexCoord).a < 0.3f) {
		discard;
	}
	FragColor = texture(texture1, TexCoord);
}
