#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texture;

uniform struct Light {
   vec3 position;
   vec3 intensities; //color of the light
} light;

uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

out vec2 texC;

void main()
{
   texC = texture;

   vec3 posGlobal = vec3(modelMatrix * vec4(position,1));

   float t = -(light.position.y / (posGlobal.y - light.position.y));
   vec3 shadowPos = vec3(light.position.x + (t * (posGlobal.x - light.position.x)) , 0.035f, light.position.z + (t * (posGlobal.z - light.position.z)));
   gl_Position = cameraMatrix * vec4(shadowPos, 1.0f);

}