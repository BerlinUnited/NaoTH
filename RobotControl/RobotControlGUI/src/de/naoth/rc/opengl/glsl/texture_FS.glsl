#version 330 core

uniform mat4 modelMatrix;
uniform sampler2D tex_1;

uniform struct Light {
   vec3 position;
   vec3 intensities; //color of the light
} light;

in vec3 fragNormal;
in vec3 fragVert;
in vec2 texCoord;


out vec4 finalColor;

void main()
{
    vec4 vertexColor = texture(tex_1, texCoord);
    //calculate normal in world coordinates
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 normal = normalize(normalMatrix * fragNormal);

    //calculate the location of this fragment (pixel) in world coordinates
    vec3 fragPosition = vec3(modelMatrix * vec4(normal, 1));

    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = light.position - fragPosition;

    //calculate the cosine of the angle of incidence
    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
    brightness = clamp(brightness, 0.2, 1);


   // finalColor = vec4(brightness * light.intensities * vertexColor.rgb, vertexColor.a);
    finalColor = vec4(brightness * light.intensities * vertexColor.rgb, vertexColor.a);
}