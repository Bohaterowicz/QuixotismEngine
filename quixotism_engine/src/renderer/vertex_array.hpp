#include <GL/glew.h>

#include "gl_buffer.hpp"
#include "gl_call.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

struct VertexArray {
  static constexpr u32 INVALID_VAO_ID = 0;
  u32 gl_id = INVALID_VAO_ID;
  size_t stride = 0;
};

inline void BindVertexArray(const VertexArray &vao) {
  GLCall(glBindVertexArray(vao.gl_id));
}

inline void BindVertexBufferToVertexArray(const VertexArray &vao,
                                          const GLBuffer &vbo,
                                          const u32 bind_slot) {
  GLCall(glVertexArrayVertexBuffer(vao.gl_id, bind_slot, vbo.gl_id, 0,
                                   vao.stride));
}

inline void BindVertexBufferToVertexArray(const VertexArray &vao,
                                          const GLBuffer &vbo,
                                          const GLBuffer &ebo,
                                          const u32 bind_slot) {
  BindVertexBufferToVertexArray(vao, vbo, bind_slot);
  GLCall(glVertexArrayElementBuffer(vao.gl_id, ebo.gl_id));
}

}  // namespace quixotism
