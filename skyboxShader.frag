
#version 330 core

in vec3 TexCoords;


out vec4 color;

uniform samplerCube skybox;

void main()
{
//    color = vec4(1.0, 0.7, 0.3, 1.0);
    color = texture(skybox, TexCoords);
}
