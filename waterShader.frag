#version 330 core
// This is a sample fragment shader.
//in vec3 fragPos;
// You can output many things. The first vec4 type output determines the color of the fragment
layout (location = 0) out vec4 color;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;

in vec2 texCoord;


void main()
{
    vec4 reflectColor = texture(reflectionTex, texCoord);
    vec4 refractionColor = texture(refractionTex, texCoord);
    // Color everything a hot blue color. An alpha of 1.0f means it is not transparent.
//    color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    color = mix(reflectColor, refractionColor, 0.5);
}
