#pragma once

#include <queue>

#include "containers/bucket_array.hpp"
#include "core/material.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class MaterialManager : public BucketArray<Material> {
 public:
  CLASS_DELETE_COPY(MaterialManager);
  MaterialManager() = default;

 private:
};
using MaterialID = MaterialManager::IdType;
}  // namespace quixotism
