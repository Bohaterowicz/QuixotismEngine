#version 420 core
out vec4 FragColor;

in vec3 VColor;
in vec2 VTexCoord;

uniform sampler2D TextureSampler;

void main()
{
    FragColor = texture(TextureSampler, VTexCoord);
}