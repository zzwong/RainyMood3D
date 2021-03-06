#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

uniform float TERRAIN_WIDTH;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform vec4 plane;

void main()
{
    vec4 worldPosition = modelview*vec4(position,1.0f);
    vec4 modelcoord = model*vec4(position,1.0f);
    
    gl_ClipDistance[0] = dot(modelcoord,plane);
    
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * worldPosition;
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = normal;
//    TexCoords = position;
//    TexCoords = position.xyz;//  / TERRAIN_WIDTH;
    TexCoords = position.xz / 2000;

}


// For Reference
//#version 330 core
//layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 normal;
//layout (location = 2) in vec2 texCoords;
//
//out vec3 Normal;
//out vec3 FragPos;
//out vec2 TexCoords;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main()
//{
//    gl_Position = projection * view *  model * vec4(position, 1.0f);
//    FragPos = vec3(model * vec4(position, 1.0f));
//    Normal = mat3(transpose(inverse(model))) * normal;
//    TexCoords = texCoords;
//}
