#include "shader_program.hpp"
#include "opengl_util.hpp"
#include "quixotism_engine.hpp"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

shader_program::shader_program(shader_program &&Other)
    : VertexShaderSourcePath{Other.VertexShaderSourcePath}, FragmentShaderSourcePath{Other.FragmentShaderSourcePath},
      UniformMap{Other.UniformMap}, ShaderTypes{Other.ShaderTypes}
{
    GetGLID() = Other.GetGLID();
    Other.GetGLID() = 0;
}

shader_program &shader_program::operator=(shader_program &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
        VertexShaderSourcePath = Other.VertexShaderSourcePath;
        FragmentShaderSourcePath = Other.FragmentShaderSourcePath;
        UniformMap = Other.UniformMap;
        ShaderTypes = Other.ShaderTypes;
    }
    return *this;
}

shader_program::~shader_program()
{
    GLCall(glDeleteProgram(GetGLID()));
}

void shader_program::Release()
{
    GLCall(glDeleteProgram(GetGLID()));
    GetGLID() = 0;
    VertexShaderSourcePath = "";
    FragmentShaderSourcePath = "";
    UniformMap.clear();
    ShaderTypes = shader_type::NONE;
}

void shader_program::AddVertexAndShaderSoruce(const std::string &VertrexSrcPath, const std::string &FragmentSrcPath)
{
    VertexShaderSourcePath = VertrexSrcPath;
    FragmentShaderSourcePath = FragmentSrcPath;
}

void shader_program::CompileShader()
{
    GLCall(GetGLID() = glCreateProgram());
    uint32 VertexShader = CompileShaderGL(VertexShaderSourcePath, GL_VERTEX_SHADER);
    uint32 FragmentShader = CompileShaderGL(FragmentShaderSourcePath, GL_FRAGMENT_SHADER);

    if (FragmentShader && VertexShader)
    {
        GLCall(glAttachShader(GetGLID(), VertexShader));
        GLCall(glAttachShader(GetGLID(), FragmentShader));
        GLCall(glLinkProgram(GetGLID()));
        GLCall(glValidateProgram(GetGLID()));

        GLCall(glDeleteShader(VertexShader));
        GLCall(glDeleteShader(FragmentShader));

        ShaderTypes = shader_type::FRAGMENT | shader_type::VERTEX;
    }
    else
    {
        GetGLID() = 0;
    }
}

uint32 shader_program::CompileShaderGL(std::string &Path, uint32 Type)
{
    auto ShaderSourceFile = GetEngine()->PlatformServices.ReadFile(Path);

    GLuint Shader = glCreateShader(Type);
    auto ShaderSrc = std::string(reinterpret_cast<const char *>(ShaderSourceFile.Content.get()), ShaderSourceFile.Size);
    const auto *ShaderSrcPtr = ShaderSrc.c_str();
    GLCall(glShaderSource(Shader, 1, &ShaderSrcPtr, nullptr));
    GLCall(glCompileShader(Shader));

    int ErrorResult = 0;
    GLCall(glGetShaderiv(Shader, GL_COMPILE_STATUS, &ErrorResult));

    if (ErrorResult == GL_FALSE)
    {
        int length = 0;
        GLCall(glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &length));
        auto message = std::make_unique<char[]>(length * sizeof(char));
        GLCall(glGetShaderInfoLog(Shader, length, &length, message.get()));
        std::string MessageType = (Type == GL_VERTEX_SHADER ? "VERTEX_SHADER" : "FRAGMENT_SHADER");
        std::string DebugMessage = "FAILED TO COMPILE SHADER: " + MessageType;
        GLDEBUG(DebugMessage.c_str());
        GLDEBUG(message.get());
        GLCall(glDeleteShader(Shader));
        return 0;
    }

    return Shader;
}

void shader_program::Bind() const
{
    GLCall(glUseProgram(GetGLID()));
}

void shader_program::Unbind() const
{
    GLCall(glUseProgram(0));
}

int32 shader_program::GetUniformLocation(const char *Name)
{
    if (UniformMap.find(Name) != UniformMap.end())
    {
        return UniformMap[Name];
    }

    GLCall(int32 UniformLocation = glGetUniformLocation(GetGLID(), Name));
    if (UniformLocation == -1)
    {
        auto Msg = std::string("Warning: Uniform ") + Name + std::string(" does not exist...");
        GLDEBUG(Msg.c_str());
    }
    UniformMap[Name] = UniformLocation;
    return UniformLocation;
}

void shader_program::SetUniform1i(const char *Name, int32 v1)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform1i(UniformLocation, v1));
    }
}

void shader_program::SetUniform1f(const char *Name, real32 v1)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform1f(UniformLocation, v1));
    }
}

void shader_program::SetUniform3f(const char *Name, real32 v1, real32 v2, real32 v3)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform3f(UniformLocation, v1, v2, v3));
    }
}

void shader_program::SetUniform3f(const char *Name, glm::vec3 &Values)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform3fv(UniformLocation, 1, glm::value_ptr(Values)));
    }
}

void shader_program::SetUniform4f(const char *Name, real32 v1, real32 v2, real32 v3, real32 v4)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform4f(UniformLocation, v1, v2, v3, v4));
    }
}

void shader_program::SetUniform4f(const char *Name, glm::vec4 &Values)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform4fv(UniformLocation, 1, glm::value_ptr(Values)));
    }
}

void shader_program::SetUniform2f(const char *Name, real32 v1, real32 v2)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        GLCall(glUniform2f(UniformLocation, v1, v2));
    }
}

void shader_program::SetUniformMtx3(const char *Name, glm::mat3 &Values)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        glUniformMatrix3fv(UniformLocation, 1, false, glm::value_ptr(Values));
    }
}

void shader_program::SetUniformMtx4(const char *Name, real32 *Values)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        glUniformMatrix4fv(UniformLocation, 1, false, Values);
    }
}

void shader_program::SetUniformMtx4(const char *Name, glm::mat4 &Values)
{
    int32 UniformLocation = GetUniformLocation(Name);
    if (UniformLocation != -1)
    {
        glUniformMatrix4fv(UniformLocation, 1, false, glm::value_ptr(Values));
    }
}