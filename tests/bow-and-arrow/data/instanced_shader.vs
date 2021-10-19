#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

invariant gl_Position;

void main() {
  gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
  //gl_Position = vec4(aPos, 1.0);
  TexCoord = aTexCoord;
}

