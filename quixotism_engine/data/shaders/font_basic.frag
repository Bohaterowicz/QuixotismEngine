#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Color;
uniform sampler2D tex_sampler;
void main() { FragColor = vec4(Color, texture(tex_sampler, TexCoord)); }