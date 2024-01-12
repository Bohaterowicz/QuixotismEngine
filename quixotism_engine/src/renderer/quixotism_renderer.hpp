#pragma once
#include <vector>
#include "math/math.hpp"

namespace quixotism {

struct TextDrawInfo {
  std::string text;
  Vec2 coord;
};

class QuixotismRenderer {
 public:
  static QuixotismRenderer& GetRenderer() {
    static QuixotismRenderer renderer{};
    return renderer;
  }

  QuixotismRenderer(const QuixotismRenderer&) = delete;
  QuixotismRenderer& operator=(const QuixotismRenderer&) = delete;

  void ClearRenderTarget();

  void Test();

  void PushText(std::string &&text, Vec2 coord);

  unsigned int VBO, VAO, EBO;
  unsigned int shaderProgram;
  unsigned int texture1;

 private:
  QuixotismRenderer();

  void DrawText();
  std::vector<TextDrawInfo> draw_text_queue;

  std::unique_ptr<u8[]> cached_text_vert_buffer;
  size_t cached_text_vert_buffer_size = 0;
};

}  // namespace quixotism
