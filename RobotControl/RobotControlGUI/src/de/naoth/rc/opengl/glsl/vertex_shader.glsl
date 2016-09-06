#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture;


uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

out vec3 fragNormal;
out vec3 fragVert;
out vec2 texCoord;

void main()
{
   fragNormal = normal;//vec3(0.0f, -0.0f, 1.0f);
   fragVert = position;
   texCoord = texture;

   gl_Position = cameraMatrix * modelMatrix * vec4(position, 1.0f);
}