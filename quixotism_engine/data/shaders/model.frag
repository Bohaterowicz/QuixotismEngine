#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

void main() {
  vec4 dsamp = texture(diffuse_tex, TexCoord);
  vec4 ssamp = texture(specular_tex, TexCoord);
  vec3 lightColor = vec3(1, 1, 1);
  float ambientStrength = 0.5;
  vec3 ambient = ambientStrength * lightColor * dsamp.xyz;
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(light_pos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor * dsamp.xyz;

  vec3 viewDir = normalize(view_pos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
  vec3 specular = spec * lightColor * ssamp.xyz;

  vec3 result = ambient + diffuse + specular;
  FragColor = vec4(result, 1.0);
}