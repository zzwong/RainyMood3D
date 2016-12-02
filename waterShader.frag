#version 330 core
// This is a sample fragment shader.
//in vec3 fragPos;
// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;


void main()
{
    // Color everything a hot blue color. An alpha of 1.0f means it is not transparent.
    color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    
}
