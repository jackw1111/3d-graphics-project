#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 5) in mat4 instanceMatrix;

out vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 model;

invariant gl_Position;

void main() {
  gl_Position = MVP * instanceMatrix * model *  vec4(aPos, 1.0);
  //gl_Position = vec4(aPos, 1.0);
  TexCoord = aTexCoord;
}

