#include <GL/glew.h>

#include "gl_buffer_manager.hpp"
#include "gl_call.hpp"

namespace quixotism {
bool GLBufferData(GLBuffer &buffer, void *data, size_t size,
                  BufferDataMode mode) {
  if (data && size) {
    GLCall(glNamedBufferData(buffer.id, size, data, mode));
    return true;
  }
  return false;
}
}  // namespace quixotism
