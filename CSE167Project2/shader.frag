#version 330 core

// This is a sample fragment shader.
in vec4 outColor;
out vec4 color;

void main()
{
    color = outColor;
}
