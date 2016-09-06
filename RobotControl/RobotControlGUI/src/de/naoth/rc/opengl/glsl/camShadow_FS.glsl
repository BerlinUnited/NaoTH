#version 330 core

uniform sampler2D image;
uniform sampler2D thresh;

in vec2 texC;


out vec4 finalColor;

void main()
{
    vec4 vertexColor = texture(image, texC);
    vec4 threshColor = texture(thresh, texC);
    if(threshColor.x == 0.0f) vertexColor.a = 0.0f;

    finalColor = vec4(vec3(0.0f, 0.0f, 0.0f), vertexColor.a * 0.5f);
}