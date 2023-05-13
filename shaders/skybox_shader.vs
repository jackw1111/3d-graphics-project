#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoords;



out vec3 TexCoords;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  