#include "impl/records.hpp"

#include <unistd.h>

#include <cassert>
#include <cstdlib>

#include <iostream>
#include <new>

namespace rsm::impl {

records::records(unsigned const block_size)
    : thread_id{static_cast<long long>(gettid())}, last{data},
      capacity{data + block_size} {}

records *records::create(unsigned const block_size) {
  // total size needed: structure size + extra array elements beyond the first
  // one
  static_assert(std::is_trivially_destructible_v<record>,
                "`record` must be trivially destructible");
  assert(block_size > 0 && "block_size must be > 0");
  size_t const size{sizeof(records) + (block_size - 1) * sizeof(record)};

  void *memory{::operator new(size)};

  records *r{new (memory) records(block_size)};

  return r;
}

void records::destroy(records *const r) noexcept {
  if (r) {
    r->~records();
    ::operator delete(r);
  }
}

void records::print_records() const {
  for (auto iter{data}; iter < last; ++iter) {
    std::cout << thread_id << ": '" << iter->desc << "', color " << iter->color
              << ", tag " << iter->tag << ": " << iter->start_ns << " -> "
              << iter->end_ns << " ~ " << (iter->end_ns - iter->start_ns)
              << " [ns]\n";
  }
}

} // namespace rsm::impl
