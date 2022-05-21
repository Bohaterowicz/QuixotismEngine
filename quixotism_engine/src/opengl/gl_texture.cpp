#include "gl_texture.hpp"
#include "opengl_util.hpp"
#include <GL/glew.h>
#include <utility>

gl_texture::gl_texture(int32 Target) : TextureTarget{Target}
{
    GLCall(glCreateTextures(TextureTarget, 1, &GetGLID()));
}

gl_texture::gl_texture(gl_texture &&Other) : TextureTarget{Other.TextureTarget}
{
    GetGLID() = Other.GetGLID();
    Other.GetGLID() = 0;
}

gl_texture &gl_texture::operator=(gl_texture &&Other)
{
    if (this != &Other)
    {
        Release();
        std::swap(GetGLID(), Other.GetGLID());
        TextureTarget = Other.TextureTarget;
    }
    return *this;
}

gl_texture::~gl_texture()
{
    GLCall(glDeleteTextures(1, &GetGLID()));
    GetGLID() = 0;
}

void gl_texture::Release()
{
    GLCall(glDeleteTextures(1, &GetGLID()));
    GetGLID() = 0;
}

void gl_texture::Bind() const
{
    GLCall(glBindTextureUnit(BindSlot, GetGLID()));
}

void gl_texture::Unbind() const
{
    GLCall(glBindTexture(TextureTarget, 0));
}

void gl_texture::SetParameterF(int32 Parameter, real32 Value)
{
    GLCall(glTextureParameterf(GetGLID(), Parameter, Value));
}

void gl_texture::SetParameterI(int32 Parameter, int32 Value)
{
    GLCall(glTextureParameteri(GetGLID(), Parameter, Value));
}