#version 330 core
layout (location = 0) in vec3 position;

//out vec2 TexCoords;
out vec4 ParticleColor;

uniform vec3 partPos;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 model;

uniform vec2 offset;
uniform vec4 color;

void main()
{
    float scale = 5.0f;
//    TexCoords = vertex.zw;
    ParticleColor = color;
//    gl_Position = projection * modelview * vec4(position.xyz / scale + partPos, 1.0f);
    gl_Position = projection * modelview * vec4(position.x/scale+partPos.x,
                                                position.y*scale+partPos.y, position.z/scale+partPos.z, 1.0f);
}
