
precision mediump float;
uniform sampler2D texture1;
uniform sampler2D texture2;
varying vec2 TexCoord;
void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	gl_FragColor = mix(texture2D(texture1, TexCoord), texture2D(texture2, TexCoord), 0.2);
}

