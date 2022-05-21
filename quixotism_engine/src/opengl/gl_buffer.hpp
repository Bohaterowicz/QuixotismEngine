#pragma once
#include "bindable.hpp"
#include "quixotism_c.hpp"
#include <memory>

#define STREAM_DRAW 0x88E0
#define STREAM_READ 0x88E1
#define STREAM_COPY 0x88E2
#define STATIC_DRAW 0x88E4
#define STATIC_READ 0x88E5
#define STATIC_COPY 0x88E6
#define DYNAMIC_DRAW 0x88E8
#define DYNAMIC_READ 0x88E9
#define DYNAMIC_COPY 0x88EA

class gl_buffer : public bindable
{
  public:
    explicit gl_buffer(int32 Target);

    gl_buffer(const gl_buffer &) = delete;
    gl_buffer(gl_buffer &&Other);

    gl_buffer &operator=(const gl_buffer &) = delete;
    gl_buffer &operator=(gl_buffer &&Other);

    ~gl_buffer() override;

    void Bind() const override;
    void Unbind() const override;

    void BufferData(void *Data, size Size, int32 Usage);
    void BufferSubData(void *Data, size Offset, size Size, int Usage);
    void ReserveData(size Size, int32 Usage);

    void Release() override;

    _NODISCARD size GetBufferSize() const
    {
        return BufferSize;
    }

    _NODISCARD const int32 &GetBindTarget() const
    {
        return BindTarget;
    }

  protected:
    int32 BindTarget = 0;
    size BufferSize = 0;
};