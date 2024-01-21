#pragma once

#include <vector>

#include "quixotism_c.hpp"

namespace quixotism {

struct LayoutElement {
  u32 type = 0;
  size_t type_size = 0;
  u32 count = 0;
  size_t offset = 0;
  bool normalize = 0;
  u32 binding_slot = 0;
};

class VertexBufferLayout {
 public:
  CLASS_DEFAULT_MOVE_AND_COPY(VertexBufferLayout);
  VertexBufferLayout() = default;

  void AddLayoutElementF(u32 count, bool normalize, u32 binding_slot);

  [[no_discard]] const std::vector<LayoutElement> &GetLayoutElements() const {
    return elements;
  }

  [[no_discard]] size_t GetStride() const { return stride; }

 private:
  std::vector<LayoutElement> elements;
  size_t stride = 0;

  void AddLayoutElement(u32 count, bool normalize, u32 type, size_t type_size,
                        u32 binding_slot);
};

}  // namespace quixotism