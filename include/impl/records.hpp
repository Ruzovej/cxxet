#pragma once

#include <memory>

#include "impl/record.hpp"

namespace rsm::impl {

struct records {
  // Static factory function to properly allocate the records structure with the
  // array inline
  static records *create(unsigned const block_size);

  // Custom delete to properly free the allocated memory
  static void destroy(records *const r) noexcept;

  struct RecordsDeleter {
    void operator()(records *r) const {
      if (r)
        destroy(r);
    }
  };

  // Private constructor for internal use only
  records(unsigned const block_size);
  ~records() noexcept = default;

  [[nodiscard]] inline bool free_capacity() const noexcept {
    return last < capacity;
  }

  [[nodiscard]] inline bool empty() const noexcept { return last == data; }

  inline void append_record(record const r) noexcept { *(last++) = r; }

  [[deprecated]] void print_records() const;

  long long const thread_id;
  std::unique_ptr<records, RecordsDeleter> next{nullptr};
  record *last, *const capacity;
  // Using a fixed-size array of 1 that will be oversized when allocated - is
  // this standard-compliant approach to the flexible array pattern?!
  record data[1];
};

} // namespace rsm::impl
