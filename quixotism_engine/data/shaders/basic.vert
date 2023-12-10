#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 ModelTransform;

void main() {
  gl_Position = MVP * vec4(inPos, 1.0);
  FragPos = vec3(ModelTransform * vec4(inPos, 1.0));
  Normal = inNormal;
}