#version 330 core
// This is a sample fragment shader.
//in vec3 fragPos;
// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

uniform sampler2D grassTex;
uniform sampler2D snowTex;
uniform sampler2D rockTex;
uniform sampler2D fieldsTex;

uniform bool wire;
uniform bool water;

void main()
{
    // Color everything a hot blue color. An alpha of 1.0f means it is not transparent.
//    color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    if(wire)
        color = vec4(0.0f, .5f, .5f, 1.0f);
    else
        color = vec4(.87f, .3f, 0.0f, 1.0f);
    
//    color = texture(grassTex, TexCoords);
}
