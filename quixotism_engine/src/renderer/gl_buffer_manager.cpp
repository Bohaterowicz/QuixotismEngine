#include "gl_buffer_manager.hpp"

#include "GL/glew.h"
#include "gl_call.hpp"

namespace quixotism {

GLBufferID GLBufferManager::Create() {
  if (free_ids.empty()) {
    return INVALID_ID;
  }

  auto id = free_ids.front();
  assert(buffer_array[id].id == GLBuffer::INVALID_BUFFER_ID);

  GLCall(glCreateBuffers(1, &buffer_array[id].id));
  assert(buffer_array[id].id != GLBuffer::INVALID_BUFFER_ID);

  free_ids.pop();
  return id;
}

std::optional<GLBuffer> GLBufferManager::Get(GLBufferID id) const {
  assert(id < buffer_array.size());
  if (Exists(id)) {
    return buffer_array[id];
  } else {
    return std::nullopt;
  }
}

bool GLBufferManager::Exists(const GLBufferID id) const {
  assert(id < ARRAY_SIZE);
  return buffer_array[id].id != GLBuffer::INVALID_BUFFER_ID;
}

void GLBufferManager::Destroy(const GLBufferID id) {
  GLCall(glDeleteBuffers(1, &buffer_array[id].id));
  free_ids.push(id);
}

GLBufferManager::~GLBufferManager() {
  for (const auto &buffer : buffer_array) {
    GLCall(glDeleteBuffers(1, &buffer.id));
  }
}

}  // namespace quixotism
