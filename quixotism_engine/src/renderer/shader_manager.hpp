#pragma once
#include <array>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "containers/bucket_array.hpp"
#include "dbg_print.hpp"
#include "math/qmath.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

enum class ShaderStageType {
  VERTEX,
  FRAGMENT,
  COMPUTE,
};

using StageSpec = std::pair<ShaderStageType, std::string>;
using ShaderStageSpec = std::vector<StageSpec>;

class Shader {
 public:
  static constexpr u32 INVALID_SHADER_ID = 0;
  Shader() = default;
  Shader(const std::string &_name) : name{_name} {}

  void SetUniform(const std::string &name, const i32 value);
  void SetUniform(const std::string &name, const r32 value);
  void SetUniform(const std::string &name, const Vec2 &value,
                  const size_t count = 1);
  void SetUniform(const std::string &name, const Vec3 &value,
                  const size_t count = 1);
  void SetUniform(const std::string &name, const Vec4 &value,
                  const size_t count = 1);
  void SetUniform(const std::string &name, const Mat2 &value,
                  const bool transpose = false, const size_t count = 1);
  void SetUniform(const std::string &name, const Mat3 &value,
                  const bool transpose = false, const size_t count = 1);
  void SetUniform(const std::string &name, const Mat4 &value,
                  const bool transpose = false, const size_t count = 1);

  const std::string &GetName() const { return name; }

  u32 id;

 private:
  i32 GetUniformLocation(const std::string &name);

  std::string name;
  std::unordered_map<std::string, i32> uniform_cache;
};

using ShaderID = u32;

class ShaderManager : public BucketArray<Shader> {
 public:
  CLASS_DELETE_COPY(ShaderManager);

  ShaderManager();

  ShaderID CreateShader(const std::string &name, const ShaderStageSpec &spec);

  [[no_discard]] ShaderID GetByName(const std::string &name);

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
  std::unordered_map<std::string, ShaderID> shader_name_map;
};

}  // namespace quixotism
