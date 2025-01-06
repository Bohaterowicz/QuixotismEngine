#pragma once
#include <variant>

#include "quixotism_c.hpp"
#include "renderer/shader_manager.hpp"
#include "texture_manager.hpp"

namespace quixotism {

class Material {
 public:
  Material() = default;
  Material(ShaderID _shader_id) : shader_id{_shader_id} {}

  [[no_discard]] u32 GetShaderId() const { return shader_id; }

  TextureID diffuse = 0;
  TextureID specular = 0;

 private:
  ShaderID shader_id = 0;
};

}  // namespace quixotism
