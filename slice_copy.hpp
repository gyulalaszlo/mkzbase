#pragma once
#include <vector>
#include "slice.hpp"

// Helper functions to copy slices to containers.

namespace mkz
{
  // Copies a constant range to a vector
  template <typename T>
  inline void copy_to(const slice<const T> s, std::vector<T>& v)
  {
    v.resize(v.size() + s.size());
    std::copy(begin(s), end(s), begin(v));
  }

  // Copy to a vector.
  template <typename T>
  inline void copy_to(const slice<T> s, std::vector<T>& v)
  {
    copy_to(s.to_const(), v);
  }
}
