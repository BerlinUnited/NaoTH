#version 330 core

uniform sampler2D tex_1;

in vec2 texCoord;


out vec4 vertexColor;

void main()
{
    vertexColor = texture(tex_1, texCoord);
}