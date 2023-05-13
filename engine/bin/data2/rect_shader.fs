#version 300 es
precision mediump float;
out vec4 FragColor;

in vec3 ourColor;

void main()
{
   FragColor = vec4(ourColor, 0.5f);
}
