#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texture;


uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

out vec2 texCoord;

void main()
{
   texCoord = texture;

   gl_Position = cameraMatrix * modelMatrix * vec4(position, 1.0f);
}