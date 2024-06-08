#pragma once

#include "core/components/camera_component.hpp"
#include "core/components/static_mesh_component.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

using ComponentId = size_t;

template <class TYPE>
constexpr bool IsCameraComponent = std::is_same_v<TYPE, CameraComponent>;
template <class TYPE>
constexpr bool IsStaticMeshComponent =
    std::is_same_v<TYPE, StaticMeshComponent>;

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
    if constexpr (IsCameraComponent<TYPE>) {
      return Add(camera_components, camera_free_ids, component);
    } else if constexpr (IsStaticMeshComponent<TYPE>) {
      return Add(static_mesh_components, static_mesh_free_ids, component);
    } else {
      assert(0);
    }
  }

  template <class TYPE>
  TYPE* GetComponent(ComponentId id) {
    if constexpr (IsCameraComponent<TYPE>) {
      return &camera_components[id];
    } else if constexpr (IsStaticMeshComponent<TYPE>) {
      return &static_mesh_components[id];
    } else {
      return nullptr;
    }
  }

 private:
  ComponentManager() {
    for (u32 i = 1; i < ARRAY_SIZE; ++i) {
      camera_free_ids.push(i);
      static_mesh_free_ids.push(i);
    }
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

  std::queue<ComponentId> camera_free_ids;
  std::array<CameraComponent, ARRAY_SIZE> camera_components;

  std::queue<ComponentId> static_mesh_free_ids;
  std::array<StaticMeshComponent, ARRAY_SIZE> static_mesh_components;
};

}  // namespace quixotism
