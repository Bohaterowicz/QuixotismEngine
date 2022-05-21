#pragma once
#include "bindable.hpp"
#include "quixotism_c.hpp"
#include <GL/glew.h>

class gl_sampler : public bindable
{
  public:
    gl_sampler();

    gl_sampler(const gl_sampler &) = delete;
    gl_sampler(gl_sampler &&Other);

    gl_sampler &operator=(const gl_sampler &) = delete;
    gl_sampler &operator=(gl_sampler &&Other);

    ~gl_sampler() override;

    void Release() override;

    void Bind() const override;
    void Unbind() const override;

    void SetParameterF(int32 Parameter, real32 Value);
    void SetParameterI(int32 Parameter, int32 Value);

    void SetFilteringMode(int32 MinFilter, int32 MagFilter);
    void SetWrapMode(int32 WrapS = GL_CLAMP_TO_EDGE, int32 WrapT = GL_CLAMP_TO_EDGE, int32 WrapR = GL_CLAMP_TO_EDGE);
    void SetAnisotropy(real32 AnisotropyLevel);
    static real32 GetMaxAnisotropy();

    void SetBindSlot(int32 Slot)
    {
        BindSlot = Slot;
    }

    _NODISCARD const int32 &GetBindSlot() const
    {
        return BindSlot;
    }

  private:
    int32 BindSlot = 0;
};