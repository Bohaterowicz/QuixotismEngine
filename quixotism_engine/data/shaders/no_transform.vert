#version 420 core
layout(location = 0) in vec3 Pos;
layout(location = 1) in vec3 Color;
layout(location = 2) in vec2 TexCoord;

out vec3 VColor;
out vec2 VTexCoord;

void main()
{
    gl_Position = vec4(Pos, 1.0);
    VColor = Color;
    VTexCoord = TexCoord;
}