#pragma once
#include <array>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

#include "quixotism_c.hpp"

namespace quixotism {

enum class ShaderStageType {
  VERTEX,
  FRAGMENT,
};

using StageSpec = std::pair<ShaderStageType, std::string>;
using ShaderStageSpec = std::vector<StageSpec>;

struct Shader {
  static constexpr u32 INVALID_SHADER_ID = 0;
  u32 id;
};

using ShaderID = u32;

class ShaderManager {
 public:
  static constexpr size_t ARRAY_SIZE = 256;
  static constexpr ShaderID INVALID_ID = 0;
  CLASS_DELETE_COPY(ShaderManager);

  ShaderManager();

  [[no_discard]] ShaderID CreateShader(const ShaderStageSpec &spec);
  [[no_discard]] std::optional<Shader> Get(const ShaderID id) const;
  [[no_discard]] bool Exists(const ShaderID id) const;

 private:
  struct GLStage {
    CLASS_DELETE_COPY(GLStage);
    explicit GLStage(ShaderStageType stage);
    GLStage(GLStage &&other);
    operator bool() const { return id != 0; }
    GLStage &operator=(GLStage &&other);
    ~GLStage();
    u32 id;
    u32 attached_shader;
  };

  std::optional<GLStage> CompileStage(const StageSpec stage) const;

  std::queue<ShaderID> free_ids;
  std::array<Shader, ARRAY_SIZE> shaders;
};

}  // namespace quixotism
