#include "shader_manager.hpp"

#include <GL/glew.h>

#include "core/quixotism_engine.hpp"
#include "gl_call.hpp"
#include "scope_guard.hpp"

namespace quixotism {

static u32 GLShaderStageType(ShaderStageType stage) {
  switch (stage) {
    case ShaderStageType::VERTEX:
      return GL_VERTEX_SHADER;
    case ShaderStageType::FRAGMENT:
      return GL_FRAGMENT_SHADER;
    default:
      assert(0);
  }
}

ShaderManager::GLStage::GLStage(ShaderStageType stage) : attached_shader{0} {
  GLCall(id = glCreateShader(GLShaderStageType(stage)));
}

ShaderManager::GLStage &ShaderManager::GLStage::operator=(GLStage &&other) {
  id = other.id;
  attached_shader = other.attached_shader;
  other.id = 0;
  other.attached_shader = 0;
  return *this;
}

ShaderManager::GLStage::GLStage(ShaderManager::GLStage &&other) {
  id = other.id;
  attached_shader = other.attached_shader;
  other.id = 0;
  other.attached_shader = 0;
}

ShaderManager::GLStage::~GLStage() {
  if (id) {
    if (attached_shader) {
      GLCall(glDetachShader(attached_shader, id));
    }
    GLCall(glDeleteShader(id));
  }
}

ShaderManager::ShaderManager() {
  for (u32 i = 1; i < ARRAY_SIZE; ++i) {
    free_ids.push(i);
  }
  Shader def{};
  shaders.fill(def);
}

ShaderID ShaderManager::CreateShader(const ShaderStageSpec &spec) {
  if (spec.empty() || free_ids.empty()) {
    return INVALID_ID;
  }

  auto id = free_ids.front();
  auto &shader = shaders[id];
  assert(shader.id == Shader::INVALID_SHADER_ID);

  GLCall(shader.id = glCreateProgram());
  assert(shader.id != Shader::INVALID_SHADER_ID);

  // IMPORTANT:We have created a shader program object succesully, if for some
  // reason we fail during compilation/attachment/verification we are gonna exit
  // with INVALID_ID, and so we are also want to delete this shader program, if
  // we succeed in creating the sahder we NEED to disengage this scope guard!
  ScopeGuard guard([&shader] {
    GLCall(glDeleteProgram(shader.id));
    shader.id = Shader::INVALID_SHADER_ID;
  });

  std::vector<GLStage> gl_stages;
  for (const auto &stage : spec) {
    if (auto gl_stage = CompileStage(stage)) {
      GLCall(glAttachShader(shader.id, (*gl_stage).id));
      gl_stages.push_back(std::move(*gl_stage));
      (*gl_stage).attached_shader = shader.id;
    } else {
      return INVALID_ID;
    }
  }

  GLCall(glLinkProgram(shader.id));

  i32 success = 0;
  GLCall(glGetProgramiv(shader.id, GL_LINK_STATUS, &success));
  if (!success) {
    i32 log_size = 0, actual_size = 0;
    GLCall(glGetProgramiv(shader.id, GL_INFO_LOG_LENGTH, &log_size));
    assert(log_size);
    std::string error_log;
    error_log.resize(log_size);
    GLCall(glGetProgramInfoLog(shader.id, log_size, &actual_size,
                               error_log.data()));
    assert(actual_size <= log_size);
    return INVALID_ID;
  }

  GLCall(glValidateProgram(shader.id));
  GLCall(glGetProgramiv(shader.id, GL_VALIDATE_STATUS, &success));
  if (!success) {
    assert(false);
  }

  guard.Disengage();
  free_ids.pop();
  return id;
}

std::optional<ShaderManager::GLStage> ShaderManager::CompileStage(
    const StageSpec stage) const {
  auto file_result =
      QuixotismEngine::GetEngine().services.read_file(stage.second.c_str());
  std::string source{reinterpret_cast<char *>(file_result.data.get()),
                     file_result.size};
  // do preprocessing here maybe?
  GLStage gl_stage(stage.first);
  if (!gl_stage) {
    return std::nullopt;
  }
  auto src_ptr = source.c_str();
  GLCall(glShaderSource(gl_stage.id, 1, &src_ptr, nullptr));
  GLCall(glCompileShader(gl_stage.id));

  i32 success = 0;
  GLCall(glGetShaderiv(gl_stage.id, GL_COMPILE_STATUS, &success));

  if (!success) {
    i32 log_size = 0, actual_size = 0;
    GLCall(glGetShaderiv(gl_stage.id, GL_INFO_LOG_LENGTH, &log_size));
    assert(log_size);
    std::string error_log;
    error_log.resize(log_size);
    GLCall(glGetShaderInfoLog(gl_stage.id, log_size, &actual_size,
                              error_log.data()));
    assert(actual_size <= log_size);
    return std::nullopt;
  }

  return gl_stage;
}

std::optional<Shader> ShaderManager::Get(const ShaderID id) const {
  assert(id < shaders.size());
  if (Exists(id)) {
    return shaders[id];
  } else {
    return std::nullopt;
  }
}

bool ShaderManager::Exists(const ShaderID id) const {
  assert(id < ARRAY_SIZE);
  return shaders[id].id != Shader::INVALID_SHADER_ID;
}

}  // namespace quixotism
