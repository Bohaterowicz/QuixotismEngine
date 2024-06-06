#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D tex_sampler;
void main() {
  FragColor = vec4(1.0f, 1.0f, 1.0f, texture(tex_sampler, TexCoord));
}