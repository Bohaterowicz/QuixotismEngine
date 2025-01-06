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
    case ShaderStageType::COMPUTE:
      return GL_COMPUTE_SHADER;
    default:
      Assert(0);
  }
}

i32 Shader::GetUniformLocation(const std::string &name) {
  if (uniform_cache.contains(name)) {
    return uniform_cache[name];
  }

  GLCall(i32 uniform_location = glGetUniformLocation(id, name.c_str()));
  if (uniform_location == -1) {
    auto Msg = std::string("Warning: Uniform ") + name +
               std::string(" does not exist.");
    DBG_PRINT(Msg.c_str());
  } else {
    uniform_cache[name] = uniform_location;
  }
  return uniform_location;
}

void Shader::SetUniform(const std::string &name, const i32 value) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniform1i(uniform, value));
  }
}

void Shader::SetUniform(const std::string &name, const r32 value) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniform1f(uniform, value));
  }
}

void Shader::SetUniform(const std::string &name, const Vec2 &value,
                        const size_t count) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniform2fv(uniform, count, value.DataPtr()));
  }
}

void Shader::SetUniform(const std::string &name, const Vec3 &value,
                        const size_t count) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniform3fv(uniform, count, value.DataPtr()));
  }
}

void Shader::SetUniform(const std::string &name, const Vec4 &value,
                        const size_t count) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniform4fv(uniform, count, value.DataPtr()));
  }
}

void Shader::SetUniform(const std::string &name, const Mat2 &value,
                        const bool transpose, const size_t count) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniformMatrix2fv(uniform, count, transpose, value.DataPtr()));
  }
}

void Shader::SetUniform(const std::string &name, const Mat3 &value,
                        const bool transpose, const size_t count) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniformMatrix3fv(uniform, count, transpose, value.DataPtr()));
  }
}

void Shader::SetUniform(const std::string &name, const Mat4 &value,
                        const bool transpose, const size_t count) {
  auto uniform = GetUniformLocation(name);
  if (uniform != -1) {
    GLCall(glUniformMatrix4fv(uniform, count, transpose, value.DataPtr()));
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

ShaderManager::ShaderManager() {}

ShaderID ShaderManager::CreateShader(const std::string &name,
                                     const ShaderStageSpec &spec) {
  if (shader_name_map.find(name) != shader_name_map.end()) {
    DBG_PRINT("Shader with name '" + name + "' already exists...");
    return Shader::INVALID_SHADER_ID;
  }
  Shader shader{name};
  GLCall(shader.id = glCreateProgram());
  Assert(shader.id != Shader::INVALID_SHADER_ID);

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
    Assert(log_size);
    std::string error_log;
    error_log.resize(log_size);
    GLCall(glGetProgramInfoLog(shader.id, log_size, &actual_size,
                               error_log.data()));
    Assert(actual_size <= log_size);
    return INVALID_ID;
  }

  GLCall(glValidateProgram(shader.id));
  GLCall(glGetProgramiv(shader.id, GL_VALIDATE_STATUS, &success));
  if (!success) {
    Assert(false);
  }

  auto id = this->Add(std::move(shader));
  if (id) {
    guard.Disengage();
    shader_name_map[name] = id;
  }
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
    Assert(log_size);
    std::string error_log;
    error_log.resize(log_size);
    GLCall(glGetShaderInfoLog(gl_stage.id, log_size, &actual_size,
                              error_log.data()));
    Assert(actual_size <= log_size);
    return std::nullopt;
  }

  return gl_stage;
}

ShaderID ShaderManager::GetByName(const std::string &name) {
  if (shader_name_map.find(name) == shader_name_map.end()) {
    return Shader::INVALID_SHADER_ID;
  } else {
    return shader_name_map[name];
  }
}

}  // namespace quixotism
