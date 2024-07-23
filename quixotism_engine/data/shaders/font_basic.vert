#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inTexCoord;
layout(location = 2) in vec3 inColor;
out vec3 TexCoord;
out vec3 Color;
void main() {
  gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0);
  TexCoord = inTexCoord;
  Color = inColor;
}