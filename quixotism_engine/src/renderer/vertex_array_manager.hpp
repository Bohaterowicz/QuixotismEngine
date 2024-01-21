#pragma once
#include <array>
#include <queue>

#include "quixotism_c.hpp"
#include "vertex_array.hpp"
#include "vertex_buffer_layout.hpp"

namespace quixotism {

using VertexArrayID = u32;

class VertexArrayManager {
 public:
  static constexpr size_t ARRAY_SIZE = 128;
  static constexpr VertexArrayID INVALID_ID = 0;

  CLASS_DELETE_COPY(VertexArrayManager);

  VertexArrayManager() {
    for (u32 i = 1; i < ARRAY_SIZE; ++i) {
      free_indicies.push(i);
    }
  }

  VertexArrayManager(VertexArrayManager &&other) noexcept {
    free_indicies = std::move(other.free_indicies);
    vao_array = std::move(other.vao_array);
    VertexArray def{};
    std::fill(other.vao_array.begin(), other.vao_array.end(), def);
  }

  VertexArrayManager &operator=(VertexArrayManager &&other) noexcept {
    free_indicies = std::move(other.free_indicies);
    vao_array = std::move(other.vao_array);
    VertexArray def{};
    std::fill(other.vao_array.begin(), other.vao_array.end(), def);
    return *this;
  }

  [[no_discard]] std::optional<VertexArrayID> Create(
      const VertexBufferLayout &layout);
  [[no_discard]] std::optional<VertexArray> Get(VertexArrayID id) const;
  [[no_discard]] bool Exists(const VertexArrayID id) const;
  void Destroy(const VertexArrayID id);

  ~VertexArrayManager();

 private:
  std::queue<VertexArrayID> free_indicies;
  std::array<VertexArray, ARRAY_SIZE> vao_array;

  void SpecifyAttributeLayout(const VertexArrayID id,
                              const VertexBufferLayout &layout);
};

}  // namespace quixotism
