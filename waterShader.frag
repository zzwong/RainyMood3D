#version 330 core
// This is a sample fragment shader.
//in vec3 fragPos;
// You can output many things. The first vec4 type output determines the color of the fragment
layout (location = 0) out vec4 color;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;

in vec4 clipSpace;

//in vec2 texCoord;

void main()
{
    vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0f + 0.5f;
    vec2 refractTexCoord = vec2(ndc.x,ndc.y);
    vec2 reflectTexCoord = vec2(ndc.x,-ndc.y);
    
    vec4 reflectColor = texture(reflectionTex, reflectTexCoord);
    vec4 refractionColor = texture(refractionTex, refractTexCoord);
    //color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    color = mix(reflectColor, refractionColor, .5f);
    //color = reflectColor;
    //color = refractionColor;
}
