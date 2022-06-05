#include "texture2d.hpp"
#include "opengl_util.hpp"
#include <GL/glew.h>

texture2d::texture2d(std::unique_ptr<uint8> ImageData, int32 Width, int32 Height, int32 ChannelCount)
    : gl_texture{GL_TEXTURE_2D}
{
    GLCall(glTextureStorage2D(GetGLID(), 1, GL_RGB8, Width, Height));
    GLCall(glTextureSubImage2D(GetGLID(), 0, 0, 0, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, ImageData.get()));
    GLCall(glTextureParameteri(GetGLID(), GL_TEXTURE_MIN_FILTER, GL_LINEAR));
}

texture2d::texture2d(uint8 *ImageData, int32 Width, int32 Height, int32 ChannelCount) : gl_texture{GL_TEXTURE_2D}
{
    GLCall(glTextureStorage2D(GetGLID(), 1, GL_RGBA8, Width, Height));
    GLCall(glTextureSubImage2D(GetGLID(), 0, 0, 0, Width, Height, GL_BGRA, GL_UNSIGNED_BYTE, ImageData));
    GLCall(glTextureParameteri(GetGLID(), GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glGenerateTextureMipmap(GetGLID()));
}