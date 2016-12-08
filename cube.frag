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
uniform int cube_color;

void main()
{
    // Color everything a hot blue color. An alpha of 1.0f means it is not transparent.
    if (cube_color == 1){
        color = vec4(1.0, 0.0, 0.0, 1.0);
    } else if (cube_color == 2){
        color = vec4(0.0, 0.0, 1.0, 1.0);
    } else if (cube_color == 3){
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
    
}
