#include "gl_sampler.hpp"
#include "opengl_util.hpp"
#include <utility>

gl_sampler::gl_sampler()
{
    GLCall(glCreateSamplers(1, &GetGLID()));
}

gl_sampler::gl_sampler(gl_sampler &&Other) : BindSlot{Other.BindSlot}
{
    GetGLID() = Other.GetGLID();
    Other.GetGLID() = 0;
}

gl_sampler &gl_sampler::operator=(gl_sampler &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
        BindSlot = Other.BindSlot;
    }
    return *this;
}

gl_sampler::~gl_sampler()
{
    GLCall(glDeleteSamplers(1, &GetGLID()));
}

void gl_sampler::Release()
{
    GLCall(glDeleteSamplers(1, &GetGLID()));
    GetGLID() = 0;
}

void gl_sampler::Bind() const
{
    GLCall(glBindSampler(BindSlot, GetGLID()));
}

void gl_sampler::Unbind() const
{
    GLCall(glBindSampler(BindSlot, 0));
}

void gl_sampler::SetParameterF(int32 Parameter, real32 Value)
{
    GLCall(glSamplerParameterf(GetGLID(), Parameter, Value));
}

void gl_sampler::SetParameterI(int32 Parameter, int32 Value)
{
    GLCall(glSamplerParameteri(GetGLID(), Parameter, Value));
}

void gl_sampler::SetFilteringMode(int32 MinFilter, int32 MagFilter)
{
    GLCall(glSamplerParameteri(GetGLID(), GL_TEXTURE_MIN_FILTER, MinFilter));
    GLCall(glSamplerParameteri(GetGLID(), GL_TEXTURE_MAG_FILTER, MagFilter));
}

void gl_sampler::SetWrapMode(int32 WrapS, int32 WrapT, int32 WrapR)
{
    GLCall(glSamplerParameteri(GetGLID(), GL_TEXTURE_WRAP_S, WrapS));
    GLCall(glSamplerParameteri(GetGLID(), GL_TEXTURE_WRAP_T, WrapT));
    GLCall(glSamplerParameteri(GetGLID(), GL_TEXTURE_WRAP_R, WrapR));
}

void gl_sampler::SetAnisotropy(real32 AnisotropyLevel)
{
    GLCall(glSamplerParameterf(GetGLID(), GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLfloat>(AnisotropyLevel)));
}

real32 gl_sampler::GetMaxAnisotropy()
{
    GLfloat MaxAniso = 0.0F;
    GLCall(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MaxAniso));
    return static_cast<real32>(MaxAniso);
}