#version 300 es
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
out vec3 Pos;
uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
	TexCoords = vertex.zw;
	Pos = vec3(vertex.x, vertex.y, 1.0);
}