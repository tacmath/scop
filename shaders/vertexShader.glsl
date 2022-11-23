#version 400 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;

out vec3 color;
out vec3 pos;

uniform vec3 Osize;
uniform vec3 Omin;

uniform mat4 matrix;
uniform mat4 model;

void main()
{
   gl_Position = matrix * model * vec4(aPos, 1.0f); //  proj * view * model *
   pos = aPos;
   color = vec3(1.0f - ((pos.x - Omin.x) / Osize.x), 1.0f  - ((pos.y- Omin.y) / Osize.y), 1.0f  - ((pos.z- Omin.z) / Osize.z));
}