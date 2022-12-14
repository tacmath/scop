#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out vec3 Normal;
out vec2 texCoord;
out vec3 currentPos;
out mat3 TBN;

uniform mat4 matrix;
uniform mat4 model;

void main()
{
   currentPos = vec3(model * vec4(aPos, 1.0f));

   gl_Position = matrix * model * vec4(aPos, 1.0f); //  proj * view * model *

 /*  mat3 modelVector = transpose(inverse(mat3(model))); // not needed if model doesn't rotate and doesn't change shape

   vec3 tangent = normalize(modelVector * aTangent);
   vec3 bitangent = normalize(modelVector * aBitangent);
   Normal = normalize(modelVector * aNormal);*/
   vec3 tangent = aTangent;
   vec3 bitangent = aBitangent;
   Normal = normalize(aNormal);
   TBN = mat3(tangent, bitangent, Normal);

   texCoord = aTex;
}