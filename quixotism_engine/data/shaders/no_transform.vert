#version 420 core
layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 TexCoord;

out vec2 VTexCoord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(Pos, 1.0);
    VTexCoord = TexCoord;
}