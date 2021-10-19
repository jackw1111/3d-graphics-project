#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

// texture sampler
uniform sampler2D texture1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;

uniform vec2 mousePosition;


void main() {
	if (texture(texture1, TexCoord).a == 0.0f) {
		discard;
	} else {
	    float dist = length(TexCoord - mousePosition);
	    if (dist>0.1f && dist<0.14f)
	    	FragColor = vec4(0.0, 1.0, 0.0f, 1.0f);
	    else
	    	if (FragPos.y == 0.0) {
	    		// water
	    		FragColor = texture(texture_diffuse3, TexCoord);
	    	} else if (FragPos.y < 0.2) {
	    		// sand
	    		FragColor = texture(texture_diffuse2, TexCoord);
	    	}
	    	else {
	    		// grass
	    		FragColor = texture(texture1, TexCoord);
	    	}
	    	
	}
}
