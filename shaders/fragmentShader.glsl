#version 400 core
out vec4 FragColor;

in vec3 color;
in vec3 pos;

uniform vec3 Osize;
uniform vec3 Omin;

uniform sampler2D Texture;
uniform float     transition;

void main()
{
      vec4 textColor = texture(Texture, vec2((pos.x - Omin.x + pos.z - Omin.z) / ((Osize.x + Osize.z)), ((pos.y - Omin.y) / Osize.y)));
      FragColor = vec4(color * (1.0f - transition) + vec3(textColor) * transition, 1.0f);
}