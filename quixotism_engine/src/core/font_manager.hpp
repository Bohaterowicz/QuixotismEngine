#pragma once

#include "containers/bucket_array.hpp"
#include "fonts/font.hpp"
#include "math/basic.hpp"
#include "quixotism_c.hpp"

namespace quixotism {

class FontManager : public BucketArray<FontSet> {
 public:
  CLASS_DELETE_COPY(FontManager);
  FontManager() = default;

  Font *GetByFontScale(IdType id, r32 scale) {
    auto *font_set = Get(id);
    if (!font_set) return nullptr;

    u32 idx = 0;
    r32 diff = Abs(scale - font_set->fonts[idx].GetScale());
    for (u32 i = 1; i < ArrayCount(FontSet::font_sizes); ++i) {
        auto new_diff = Abs(scale - font_set->fonts[i].GetScale());
        if (new_diff < diff) {
            diff = new_diff;
            idx = i;
        }
    }

    return &font_set->fonts[idx];
  }

  Font *GetByFontSize(IdType id, r32 size) {
    auto *font_set = Get(id);
    if (!font_set) return nullptr;

    u32 idx = 0;
    r32 diff = Abs(size - font_set->font_sizes[idx]);
    for (u32 i = 1; i < ArrayCount(FontSet::font_sizes); ++i) {
      auto new_diff = Abs(size - font_set->font_sizes[i]);
      if (new_diff < diff) {
        diff = new_diff;
        idx = i;
      }
    }

    return &font_set->fonts[idx];
  }

  IdType GetFontIdxByScale(IdType id, r32 scale) {
    auto *font_set = Get(id);
    if (!font_set) return 0;

    u32 idx = 0;
    r32 diff = Abs(scale - font_set->fonts[idx].GetScale());
    for (u32 i = 1; i < ArrayCount(FontSet::font_sizes); ++i) {
      auto new_diff = Abs(scale - font_set->fonts[i].GetScale());
      if (new_diff < diff) {
        diff = new_diff;
        idx = i;
      }
    }

    return idx;
  }

  IdType GetDefault() {
    Assert(Exists(1));
    return 1;
  }

 private:
};
using FontID = FontManager::IdType;
}  // namespace quixotism
