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

}  // namespace quixotism
