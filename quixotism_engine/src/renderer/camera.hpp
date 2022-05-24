#pragma once
#include "components/transform.hpp"
#include "entity.hpp"
#include "quixotism_c.hpp"
#include <glm/glm.hpp>

enum class projection_type : int8
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

struct perspective_projection_info
{
    real32 NearPlane = 0.0F;
    real32 FarPlane = 0.0F;
    real32 FOV = 0.0F;
};

struct orthographic_projection_info
{
    real32 NearPlane = 0.0F;
    real32 FarPlane = 0.0F;
    real32 Right = 0.0F;
    real32 Left = 0.0F;
    real32 Top = 0.0F;
    real32 Bottom = 0.0F;
};

class camera : public entity
{
  public:
    camera();
    explicit camera(perspective_projection_info &Info);
    explicit camera(orthographic_projection_info &Info);

    void ChangeProjectionType(projection_type Type);
    void SetPerspectiveProjectionInfo(perspective_projection_info &Info);
    void SetOrthographicProjectionInfo(orthographic_projection_info &Info);

    _NODISCARD perspective_projection_info &GetPerspectiveProjectionInfo()
    {
        return PerspectiveProjectionInfo;
    }

    _NODISCARD orthographic_projection_info &GetOrthographicProjectionInfo()
    {
        return OrthographicProjectionInfo;
    }

    _NODISCARD const glm::mat4 &GetProjectionMatrix() const
    {
        return CurrentProjectionMtx;
    }

    _NODISCARD glm::mat4 GetViewMatrix();

    void ProcessInput(engine_input &Input, real32 DeltaTime) override;

  private:
    void InitPerspectiveProjectionDefault();
    void InitOrthographicProjectionDefault();
    _NODISCARD glm::mat4 CalculateProjectionMatrix() const;
    _NODISCARD glm::mat4 CalculatePerspectiveProjectionMatrix() const;
    _NODISCARD glm::mat4 ClaculateOrthographicProjectionMatrix() const;

    projection_type ProjectionType = projection_type::PERSPECTIVE;
    perspective_projection_info PerspectiveProjectionInfo = {};
    orthographic_projection_info OrthographicProjectionInfo = {};
    glm::mat4 CurrentProjectionMtx = {};
};