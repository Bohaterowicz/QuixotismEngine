#pragma once
#include "bindable.hpp"
#include "quixotism_c.hpp"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

/*
template<class T> inline T operator~ (T a) { return (T)~(int)a; }
template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }
*/

enum class shader_type : uint8
{
    NONE = 0,
    VERTEX = 1,
    FRAGMENT = 2,
    GEOMETRY = 4,
    COMPUTE = 8,
    TESS_CONTROL = 16,
    TESS_EVALUATION = 32,
};

inline shader_type operator|(shader_type A, shader_type B)
{
    return static_cast<shader_type>(static_cast<int8>(A) | static_cast<int8>(B));
}
inline shader_type operator&(shader_type A, shader_type B)
{
    return static_cast<shader_type>(static_cast<int8>(A) & static_cast<int8>(B));
}

class shader_program : public bindable
{
  public:
    shader_program() = default;
    shader_program(const shader_program &) = delete;
    shader_program(shader_program &&Other);

    shader_program &operator=(const shader_program &) = delete;
    shader_program &operator=(shader_program &&Other);

    ~shader_program() override;

    _NODISCARD shader_type GetShaderTypes() const
    {
        return ShaderTypes;
    }

    void AddVertexAndShaderSoruce(const std::string &VertrexSrcPath, const std::string &FragmentSrcPath);
    void CompileShader();

    void Bind() const override;
    void Unbind() const override;

    void Release() override;

    void SetUniform1i(const char *Name, int32 v1);
    void SetUniform1f(const char *Name, real32 v1);
    void SetUniform3f(const char *Name, real32 v1, real32 v2, real32 v3);
    void SetUniform3f(const char *Name, glm::vec3 &Values);
    void SetUniform4f(const char *Name, real32 v1, real32 v2, real32 v3, real32 v4);
    void SetUniform4f(const char *Name, glm::vec4 &Values);
    void SetUniform2f(const char *Name, real32 v1, real32 v2);
    void SetUniformMtx3(const char *Name, glm::mat3 &Values);
    void SetUniformMtx4(const char *Name, real32 *Values);
    void SetUniformMtx4(const char *Name, glm::mat4 &Values);
    int32 GetUniformLocation(const char *Name);

  protected:
  private:
    uint32 CompileShaderGL(std::string &Source, uint32 Type);
    std::string VertexShaderSourcePath;
    std::string FragmentShaderSourcePath;
    std::unordered_map<const char *, int32> UniformMap;
    shader_type ShaderTypes = shader_type::NONE;
};