#version 330 core

uniform sampler2D tex_1;

in vec2 texC;

out vec4 finalColor;

void main()
{
    vec4 vertexColor = texture(tex_1, texC);

    finalColor = vec4(vec3(0.2f, 0.0f, 0.0f), vertexColor.a);
}