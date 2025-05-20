#include "impl/records.hpp"

#include <unistd.h>

#include <cassert>
#include <cstdlib>

#include <iostream>
#include <new>

namespace rsm::impl {

void *records::operator new(std::size_t base_size, unsigned const block_size) {
  // total size needed: structure size + extra array elements beyond the first
  // one
  static_assert(std::is_trivially_destructible_v<record>,
                "`record` must be trivially destructible");
  assert(block_size > 0 && "block_size must be > 0");
  size_t const total_size{base_size + (block_size - 1) * sizeof(record)};

  // Use global operator new to allocate memory:
  return ::operator new(total_size);
}

void records::operator delete(void *ptr) noexcept {
  if (ptr) {
    // Use global operator delete to free memory:
    ::operator delete(ptr);
  }
}

std::unique_ptr<records> records::create(unsigned const block_size) {
  return std::unique_ptr<records>(new (block_size) records(block_size));
}

records::records(unsigned const block_size)
    : thread_id{static_cast<long long>(gettid())}, last{data},
      capacity{data + block_size} {}

void records::print_records() const {
  for (auto iter{data}; iter < last; ++iter) {
    std::cout << thread_id << ": '" << iter->desc << "', color " << iter->color
              << ", tag " << iter->tag << ": " << iter->start_ns << " -> "
              << iter->end_ns << " ~ " << (iter->end_ns - iter->start_ns)
              << " [ns]\n";
  }
}

} // namespace rsm::impl
