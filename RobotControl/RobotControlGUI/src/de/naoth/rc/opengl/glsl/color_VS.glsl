#version 330 core
layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 normal;


uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

out vec3 fragNormal;
out vec3 fragVert;

void main()
{
   fragNormal = normal;
   fragVert = position;

   gl_Position = cameraMatrix * modelMatrix * vec4(position, 1.0f);
}