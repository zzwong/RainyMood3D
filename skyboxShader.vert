#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 modelview;

// Send these texture coordinates to the fragment shader for skybox.
out vec3 TexCoords;

void main()
{
    // From previous assignment:
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
    TexCoords = position;
}
