#version 400 core
out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube skybox;

void main()
{
    vec3 color = texture(skybox, texCoord).rgb;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
  
    FragColor = vec4(color, 1.0);
}