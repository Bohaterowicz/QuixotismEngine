#version 330 core
layout(location = 0) in vec3 inPos;
uniform mat4 view;
uniform mat4 projection;
uniform float scale;
void main() {
  mat4 offset;
  offset[0][0] = 1;
  offset[1][1] = 1;
  offset[2][2] = 1;
  offset[3] = vec4(0.85, -0.9, 0.0, 1.0);
  gl_Position = offset * projection * view * vec4(inPos * scale, 1.0);
}