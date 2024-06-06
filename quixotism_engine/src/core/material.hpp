#pragma once
#include <variant>

#include "quixotism_c.hpp"
#include "renderer/quixotism_renderer.hpp"

namespace quixotism {

class mat1 {
  void Set(void *uniforms)[auto r = reinterpret_cast<*>(uniforms);]
};

class mat2 {};

class Material {
 public:
  Material() = default;

  template <class T>
  void SetUniform(const std::string &name, const T &value,
                  const size_t count = 1) {
    auto *shader = QuixotismRenderer::GetRenderer().shader_mgr.Get(shader_id);
    if (shader) {
      shader->SetUniform(name, value, count);
    }
  }

 private:
  std::variant<mat1, mat2> mat;
  u32 shader_id = 0;
};

}  // namespace quixotism
