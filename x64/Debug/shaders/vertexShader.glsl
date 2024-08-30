#version 400 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTex;

//out vec3 color;
//out vec2 texCoord;
out vec3 pos;

uniform mat4 matrix;
uniform mat4 model;

void main()
{
   gl_Position = matrix * model * vec4(aPos, 1.0f); //  proj * view * model *
   pos = aPos;
//   color = aColor;
//   texCoord = aTex;
}