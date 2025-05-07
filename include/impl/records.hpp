#pragma once

#include "impl/record.hpp"

namespace rsm::impl {

struct records {
  records(unsigned const block_size);
  ~records() noexcept;

  [[nodiscard]] inline bool free_capacity() const noexcept {
    return last < capacity;
  }

  inline void append_record(record const r) noexcept { *(last++) = r; }

  void print_records() const;

  unsigned long long const thread_id;
  records *next{nullptr};
  record *const first, *last, *const capacity;
};

} // namespace rsm::impl
