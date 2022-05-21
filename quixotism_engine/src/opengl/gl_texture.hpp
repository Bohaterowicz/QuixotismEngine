#pragma once
#include "bindable.hpp"
#include "quixotism_c.hpp"

class gl_texture : public bindable
{
  public:
    explicit gl_texture(int32 Target);

    gl_texture(const gl_texture &) = delete;
    gl_texture(gl_texture &&Other);

    gl_texture &operator=(const gl_texture &) = delete;
    gl_texture &operator=(gl_texture &&Other);

    ~gl_texture() override;

    void Bind() const override;
    void Unbind() const override;

    void SetParameterF(int32 Parameter, real32 Value);
    void SetParameterI(int32 Parameter, int32 Value);

    void Release() override;

    void SetBindSlot(int32 Slot)
    {
        BindSlot = Slot;
    }

    _NODISCARD const int32 &GetBindSlot() const
    {
        return BindSlot;
    }

  protected:
    int32 TextureTarget = 0;
    int32 BindSlot = 0;
};