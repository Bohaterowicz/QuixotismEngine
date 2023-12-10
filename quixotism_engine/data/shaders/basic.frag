#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 LightPos;

void main() {
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(LightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
  float ambient_strength = 0.3;
  vec3 ambient = ambient_strength * vec3(1.0, 1.0, 1.0);
  vec3 result = (ambient + diffuse) * vec3(0.5, 0.5, 0.0);
  FragColor = vec4(result, 1.0f);
}