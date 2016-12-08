#version 330 core
layout (location = 0) in vec3 position;

const vec2 ndc = vec2(.5f,.5f);

out vec2 TexCoords;

void main()
{
    TexCoords = position.xy*ndc+ndc;
    gl_Position = vec4(position.xy,0.0f,1.0f);
}
