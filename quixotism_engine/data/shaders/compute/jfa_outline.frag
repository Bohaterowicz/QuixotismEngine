#version 430 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D jfa_texture;

float distanceSquared(vec2 p1, vec2 p2) {
  vec2 diff = vec2(p1 - p2);
  return dot(diff, diff);
}

void main() {
  vec2 jfa_position = texture(jfa_texture, TexCoord).xy * vec2(800, 600);

  if (jfa_position.x < 0) discard;

  float dist = distanceSquared(jfa_position, TexCoord * vec2(800, 600));

  if (dist <= 10) {
    FragColor = vec4(1, 1, 0, 1);
  }
}