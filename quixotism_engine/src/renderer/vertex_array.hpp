#include <GL/glew.h>

#include "gl_buffer.hpp"
#include "gl_call.hpp"
#include "quixotism_c.hpp"
#include "vertex_buffer_layout.hpp"

namespace quixotism {

struct VertexArray {
  static constexpr u32 INVALID_VAO_ID = 0;
  u32 id = INVALID_VAO_ID;
  size_t stride = 0;
  VertexBufferLayout layout;
};

inline void BindVertexArray(const VertexArray &vao) {
  GLCall(glBindVertexArray(vao.id));
}

inline void BindVertexBufferToVertexArray(const VertexArray &vao,
                                          const GLBuffer &vbo,
                                          const u32 bind_slot) {
  GLCall(glVertexArrayVertexBuffer(vao.id, bind_slot, vbo.id, 0, vao.stride));
}

inline void BindVertexBufferToVertexArray(const VertexArray &vao,
                                          const GLBuffer &vbo,
                                          const GLBuffer &ebo,
                                          const u32 bind_slot) {
  BindVertexBufferToVertexArray(vao, vbo, bind_slot);
  GLCall(glVertexArrayElementBuffer(vao.id, ebo.id));
}

}  // namespace quixotism
