#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

//out vec2 texCoord;

out vec4 clipSpace;

//out vec3 fragPos;

uniform vec4 plane;

void main()
{
    vec4 worldPosition = modelview*vec4(position,1.0);
    
    clipSpace = projection * worldPosition;
    
    //Tell opengl to clip things
    gl_ClipDistance[0] = dot(worldPosition,plane);
    
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = clipSpace;
    //texCoord = vec2(position.x/2 + .5, position.y/2 + .5);
    
}
