#pragma once
#include "quixotism_c.hpp"

namespace quixotism {

class GLSampler {
 public:
  CLASS_DELETE_COPY(GLSampler);
  CLASS_DEFAULT_MOVE(GLSampler);
  GLSampler() = default;
  GLSampler(u32 p_id) : id(p_id) {}

  u32 Id() const { return id; }

  void BindUnit(u32 unit);

 private:
  u32 id;
};

GLSampler CreateSampler();

}  // namespace quixotism
