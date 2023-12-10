#pragma once

namespace quixotism {

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

  unsigned int VBO, VAO, EBO;
  unsigned int shaderProgram;
  unsigned int texture1;

 private:
  QuixotismRenderer();
};

}  // namespace quixotism
