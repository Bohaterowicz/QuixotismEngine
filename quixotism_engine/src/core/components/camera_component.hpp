#pragma once

#include "core/components/component.hpp"
#include "math/qmath.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

enum class ProjectionType {
  ORTOGRAPHIC,
  PERSPECTIVE,
};

class CameraComponent : public Component {
 public:
  CameraComponent()
      : Component{},
        fov{DegToRad(45)},
        fov_y{HorizontalToVerticalFOV(fov)},
        aspect_ratio{1},
        near_plane{0.01},
        far_plane{1000.0} {}

  CameraComponent(r32 fov, r32 aspect_ratio, r32 near, r32 far)
      : Component{},
        fov{fov},
        fov_y{HorizontalToVerticalFOV(fov)},
        aspect_ratio{aspect_ratio},
        near_plane{near},
        far_plane{far} {}

  Mat4 GetProjectionMatrix() const {
    Mat4 proj{0.0};
    auto tan_half_fov_y = Tan(fov_y / 2.0f);
    proj[0].x = 1.0f / (aspect_ratio * tan_half_fov_y);
    proj[1].y = 1.0f / tan_half_fov_y;
    proj[2].z = -((far_plane + near_plane) / (far_plane - near_plane));
    proj[2].w = -1.0f;
    proj[3].z = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    return proj;
  }

  static ComponentType Type() { return ComponentType::CAMERA; }

 private:
  ProjectionType projection = ProjectionType::PERSPECTIVE;
  r32 fov;
  r32 fov_y;
  r32 aspect_ratio;
  r32 near_plane;
  r32 far_plane;

  r32 HorizontalToVerticalFOV(r32 fov) { return fov; }
};
}  // namespace quixotism
