#pragma once

#include "core/entity.hpp"
#include "math/math.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

enum class ProjectionType {
  ORTOGRAPHIC,
  PERSPECTIVE,
};

class Camera : public Entity {
 public:
  Camera()
      : Entity{},
        fov{DegToRad(45)},
        fov_y{HorizontalToVerticalFOV(fov)},
        aspect_ratio{1},
        near_plane{0.01},
        far_plane{1000.0} {}

  Camera(r32 fov, r32 aspect_ratio, r32 near, r32 far)
      : Entity{},
        fov{fov},
        fov_y{HorizontalToVerticalFOV(fov)},
        aspect_ratio{aspect_ratio},
        near_plane{near},
        far_plane{far} {}

  Mat4 GetViewMatrix() const {
    Mat4 pos{1.0};
    pos[3] = Vec4{-transform.position, 1.0};

    auto forward = transform.Forward();
    auto right = transform.Right();
    auto local_up = transform.LocalUp();

    auto rot = Mat4(1.0F);
    rot[0] = Vec4(right, 0.0F);
    rot[1] = Vec4(local_up, 0.0F);
    rot[2] = Vec4(-forward, 0.0F);

    return rot.T() * pos;
  }

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
