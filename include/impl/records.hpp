#pragma once

#include <memory>

#include "impl/record.hpp"

namespace rsm::impl {

struct records {
  records(unsigned const block_size);
  ~records() noexcept = default;

  [[nodiscard]] inline bool free_capacity() const noexcept {
    return last < capacity;
  }

  [[nodiscard]] inline bool empty() const noexcept {
    return last == first.get();
  }

  inline void append_record(record const r) noexcept { *(last++) = r; }

  [[deprecated]] void print_records() const;

  long long const thread_id;
  std::unique_ptr<records> next{nullptr};
  std::unique_ptr<record[]>
      first; // TODO optimize this (via non-std extension?!) so this array is
             // part of this structure ...
  record *last, *const capacity;
};

} // namespace rsm::impl
