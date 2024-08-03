#version 330 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;
void main() {
  gl_Position = projection * ((view * vec4(inPos, 1.0)) + vec4(0, 0, -2, 0));
  Color = inColor;
}