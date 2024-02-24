#pragma once

#include <queue>

#include "containers/bucket_array.hpp"
#include "core/static_mesh.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class StaticMeshManager : public BucketArray<StaticMesh> {
 public:
  CLASS_DELETE_COPY(StaticMeshManager);
  StaticMeshManager() = default;

 private:
};
using StaticMeshId = StaticMeshManager::IdType;
}  // namespace quixotism
