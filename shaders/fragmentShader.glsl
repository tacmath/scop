#version 400 core
out vec4 FragColor;

//in vec3 color;
//in vec2 texCoord;
in vec3 pos;

uniform vec3 Osize;
uniform vec3 Omin;

uniform sampler2D Texture;
uniform float     transition;

void main()
{
//    FragColor = texture(tex0, texCoord);
//    FragColor = vec4(color, 1.0f);
//    FragColor = vec4(transition, transition, transition, 1.0f);
//      FragColor = texture(tex0, vec2(((pos.x + 3) / Osize.x), (pos.y / Osize.y)));
      vec4 textColor = texture(Texture, vec2((pos.x - Omin.x + pos.z - Omin.z) / ((Osize.x + Osize.z)), ((pos.y - Omin.y) / Osize.y)));
      FragColor = vec4(1.0f - ((pos.x - Omin.x) / Osize.x), 1.0f  - ((pos.y- Omin.y) / Osize.y), 1.0f  - ((pos.z- Omin.z) / Osize.z), 1.0f) * (1 - transition) + textColor * transition;
}