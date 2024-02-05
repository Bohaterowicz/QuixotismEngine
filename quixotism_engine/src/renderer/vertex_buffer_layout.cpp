#include "vertex_buffer_layout.hpp"

#include <GL/glew.h>

namespace quixotism {

void VertexBufferLayout::AddLayoutElementF(u32 count, bool normalize,
                                           u32 binding_slot) {
  AddLayoutElement(count, normalize, GL_FLOAT, sizeof(GLfloat), binding_slot);
}

void VertexBufferLayout::AddLayoutElement(u32 count, bool normalize, u32 type,
                                          size_t type_size, u32 binding_slot) {
  elements.emplace_back(type, type_size, count, stride, normalize,
                        binding_slot);
  stride += type_size * count;
}

std::pair<std::unique_ptr<u8[]>, size_t> SerializeVertexDataByLayout(
    const std::vector<void *> &vertex_data_buffers,
    const std::vector<u32 *> &vertex_index_buffers, u32 vertex_count,
    const VertexBufferLayout &layout) {
  size_t vert_size = 0;
  for (const auto &element : layout.GetLayoutElements()) {
    vert_size += element.count * element.type_size;
  }
  size_t size = vert_size * vertex_count;
  auto buffer = std::make_unique<u8[]>(size);
  auto dst = buffer.get();
  auto &elements = layout.GetLayoutElements();
  for (u32 i = 0; i < vertex_count; ++i) {
    for (auto j = 0; j < layout.GetLayoutElements().size(); ++j) {
      auto src = (static_cast<r32 *>(vertex_data_buffers[j])) +
                 (*(vertex_index_buffers[j] + i) * elements[j].count);
      size_t copy_size = elements[j].count * sizeof(r32);
      std::memcpy(dst, src, copy_size);
      dst += copy_size;
    }
  }

  return std::make_pair(std::move(buffer), size);
}

std::unique_ptr<u32[]> GenerateSerialIndexBuffer(u32 vertex_count) {
  auto buffer = std::make_unique<u32[]>(vertex_count);
  auto *ptr = buffer.get();

  for (u32 i = 0; i < vertex_count; ++i) {
    ptr[i] = i;
  }

  return std::move(buffer);
}

}  // namespace quixotism
