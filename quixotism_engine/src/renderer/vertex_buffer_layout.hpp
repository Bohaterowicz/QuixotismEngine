#pragma once

#include <memory>
#include <utility>
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

std::pair<std::unique_ptr<u8[]>, size_t> SerializeVertexDataByLayout(
    const std::vector<void *> &vertex_data_buffers,
    const std::vector<u32 *> &vertex_index_buffers, u32 vertex_count,
    const VertexBufferLayout &layout);

std::unique_ptr<u32[]> GenerateSerialIndexBuffer(u32 vertex_count);
}  // namespace quixotism