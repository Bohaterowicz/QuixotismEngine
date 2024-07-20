#pragma once

#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

#include "quixotism_c.hpp"

namespace quixotism {

template <class T, class _IdType = u64>
class BucketArray {
 public:
  using IdType = _IdType;
  CLASS_DELETE_COPY(BucketArray);
  static constexpr size_t BUCKET_SIZE = 256;
  static constexpr IdType INVALID_ID = 0;

  BucketArray() {
    elements = std::make_unique<T[]>(BUCKET_SIZE);
    currently_used = 1;
    free_ids.reserve(BUCKET_SIZE);
  }

  bool Exists(const IdType id) const {
    Assert(id < BUCKET_SIZE);
    if (id == INVALID_ID || id >= currently_used) return false;
    auto result = std::find(free_ids.begin(), free_ids.end(), id);
    return (result == free_ids.end());
  }

  IdType Add(T&& element) {
    if (free_ids.empty()) {
      if (currently_used == BUCKET_SIZE) {
        return INVALID_ID;
      }
      elements[currently_used] = std::move(element);
      return currently_used++;
    } else {
      auto id = free_ids.front();
      elements[id] = std::move(element);
      free_ids.erase(free_ids.cbegin());
      return id;
    }
  }

  IdType Clone(const IdType id) {
    auto* element = Get(id);
    if (!element) return INVALID_ID;
    auto cloned = *element;
    return Add(std::move(cloned));
  }

  void Remove(const IdType id) {
    if (Exists(id)) {
      elements[id] = {};
      free_ids.push_back(id);
    }
  }

  [[no_discard]] T* Get(const IdType id) {
    if (Exists(id)) {
      return &elements[id];
    } else {
      return nullptr;
    }
  }

  [[no_discard]] const T* Get(const IdType id) const {
    if (Exists(id)) {
      return &elements[id];
    } else {
      return nullptr;
    }
  }

  class Iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;
    Iterator() noexcept {};
    Iterator(const Iterator& other) noexcept {
      m_ptr = other.m_ptr;
      m_start = other.m_start;
      m_end = other.m_end;
      m_free_ids = other.m_free_ids;
    }
    Iterator(Iterator&& other) noexcept {
      m_ptr = other.m_ptr;
      m_start = other.m_start;
      m_end = other.m_end;
      m_free_ids = other.m_free_ids;
      other.m_ptr = nullptr;
      other.m_start = nullptr;
      other.m_end = nullptr;
      other.m_free_ids = nullptr;
    }
    Iterator(pointer ptr, pointer end,
             const std::vector<IdType>* free_ids) noexcept
        : m_ptr{ptr}, m_end{end}, m_free_ids{free_ids} {
      while (m_ptr != m_end &&
             std::find(m_free_ids->begin(), m_free_ids->end(),
                       m_ptr - m_start) != m_free_ids->end()) {
        ++m_ptr;
      }
    }

    Iterator& operator=(const Iterator& other) noexcept {
      m_ptr = other.m_ptr;
      m_start = other.m_start;
      m_end = other.m_end;
      m_free_ids = other.m_free_ids;
      return *this;
    }

    Iterator& operator=(Iterator&& other) noexcept {
      m_ptr = other.m_ptr;
      m_start = other.m_start;
      m_end = other.m_end;
      m_free_ids = other.m_free_ids;
      other.m_ptr = nullptr;
      m_start = nullptr;
      other.m_end = nullptr;
      other.m_free_ids = nullptr;
      return *this;
    }

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }
    Iterator& operator++() {
      do {
        ++m_ptr;
      } while (m_ptr != m_end &&
               std::find(m_free_ids->begin(), m_free_ids->end(),
                         m_ptr - m_start) != m_free_ids->end());
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const Iterator& a, const Iterator& b) {
      return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const Iterator& a, const Iterator& b) {
      return a.m_ptr != b.m_ptr;
    };

   private:
    pointer m_ptr, m_start, m_end;
    const std::vector<IdType>* m_free_ids;
  };
  static_assert(std::forward_iterator<Iterator>);

  Iterator begin() {
    return Iterator{elements.get() + 1, elements.get() + currently_used,
                    &free_ids};
  }
  Iterator end() {
    return Iterator{elements.get() + currently_used,
                    elements.get() + currently_used, &free_ids};
  }

 protected:
  std::unique_ptr<T[]> elements;
  size_t currently_used;
  std::vector<IdType> free_ids;
};

}  // namespace quixotism
