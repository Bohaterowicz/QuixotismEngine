#pragma once

#include "core/components/camera.hpp"
#include "core/components/transform.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

using ComponentId = size_t;

template <class TYPE>
constexpr bool IsTransformComponent = std::is_same_v<TYPE, TransformComponent>;
template <class TYPE>
constexpr bool IsCameraComponent = std::is_same_v<TYPE, CameraComponent>;

class ComponentManager {
 public:
  CLASS_DELETE_COPY(ComponentManager);
  static constexpr size_t ARRAY_SIZE = 256;
  static constexpr ComponentId INVALID_ID = 0;

  static ComponentManager& GetInstance() {
    static ComponentManager mgr{};
    return mgr;
  }

  template <class TYPE>
  ComponentId Add(TYPE& component) {
    if constexpr (IsTransformComponent<TYPE>) {
      return Add(transform_components, transform_free_ids, component);
    } else if constexpr (IsCameraComponent<TYPE>) {
      return Add(camera_components, camera_free_ids, component);
    } else {
      assert(0);
    }
  }

  template <class TYPE>
  TYPE& GetComponent(ComponentId id) {
    if constexpr(IsTransformComponent<TYPE>) {
      return transform_components[id];
    } else if constexpr (IsCameraComponent<TYPE>) {
      return camera_components[id];
    } else {
      assert(0);
    }
  }

 private:
  ComponentManager() {
    for (u32 i = 1; i < ARRAY_SIZE; ++i) {
      transform_free_ids.push(i);
      camera_free_ids.push(i);
    }
    TransformComponent transform{};
    transform_components.fill(transform);

    CameraComponent cam{};
    camera_components.fill(cam);
  }

  template <class T>
  ComponentId Add(std::array<T, ARRAY_SIZE>& arr,
                  std::queue<ComponentId>& free_ids, T& component) {
    if (free_ids.empty()) {
      return INVALID_ID;
    }

    auto id = free_ids.front();
    arr[id] = component;
    free_ids.pop();
    return id;
  }

  std::queue<ComponentId> transform_free_ids;
  std::array<TransformComponent, ARRAY_SIZE> transform_components;

  std::queue<ComponentId> camera_free_ids;
  std::array<CameraComponent, ARRAY_SIZE> camera_components;
};

}  // namespace quixotism
