#pragma once
#include "quixotism_c.hpp"

class bindable
{
  public:
    bindable() = default;
    bindable(const bindable &) = delete;
    bindable(bindable &&) = delete;

    bindable &operator=(const bindable &) = delete;
    bindable &operator=(bindable &&) = delete;

    virtual ~bindable() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void Release() = 0;

    _NODISCARD uint32 &GetGLID()
    {
        return GLID;
    }

    _NODISCARD const uint32 &GetGLID() const
    {
        return GLID;
    }

  private:
    uint32 GLID = 0;
};