#pragma once

#include "core/components/component.hpp"
#include "math/math.hpp"
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
    Mat4 proj{1.0};
    auto focal_length = 1 / Tan(fov_y / 2);
    proj[0].x = focal_length / aspect_ratio;
    proj[1].y = focal_length;
    proj[2].z = ((far_plane + near_plane) / (near_plane - far_plane));
    proj[2].w = -1;
    proj[3].z = (2 * far_plane * near_plane) / (near_plane - far_plane);
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
