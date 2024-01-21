#pragma once
#include <array>
#include <queue>

#include "gl_buffer.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

using GLBufferID = u32;

class GLBufferManager {
 public:
  static constexpr size_t ARRAY_SIZE = 256;
  static constexpr GLBufferID INVALID_ID = 0;
  CLASS_DELETE_COPY(GLBufferManager);
  GLBufferManager() {
    for (u32 i = 1; i < ARRAY_SIZE; ++i) {
      free_ids.push(i);
    }
    GLBuffer def{};
    buffer_array.fill(def);
  }

  GLBufferManager(GLBufferManager &&other) noexcept {
    free_ids = std::move(other.free_ids);
    buffer_array = std::move(other.buffer_array);
    GLBuffer def{};
    std::fill(other.buffer_array.begin(), other.buffer_array.end(), def);
  }

  GLBufferManager &operator=(GLBufferManager &&other) noexcept {
    free_ids = std::move(other.free_ids);
    buffer_array = std::move(other.buffer_array);
    GLBuffer def{};
    std::fill(other.buffer_array.begin(), other.buffer_array.end(), def);
    return *this;
  }

  [[no_discard]] GLBufferID Create();
  [[no_discard]] std::optional<GLBuffer> Get(GLBufferID id) const;
  [[no_discard]] bool Exists(const GLBufferID id) const;
  void Destroy(const GLBufferID id);

  ~GLBufferManager();

 private:
  // for now we are gonna use std::array with std::queue, but in the future we
  // are gonna probabyl want to create a custom BucketArray, since there may be
  // many buffers, so we are possibly gonna want more flexibility with
  // allocating buffers?
  std::queue<GLBufferID> free_ids;
  std::array<GLBuffer, ARRAY_SIZE> buffer_array;
};

}  // namespace quixotism
