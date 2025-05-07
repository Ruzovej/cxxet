#include "impl/records.hpp"

#include <unistd.h>

#include <iostream>

namespace rsm::impl {

records::records(unsigned const block_size)
    : thread_id{static_cast<unsigned long long>(gettid())},
      first{new record[block_size]}, last{first}, capacity{first + block_size} {
}

records::~records() noexcept { delete[] first; }

void records::print_records() const {
  for (auto iter{first}; iter < last; ++iter) {
    std::cout << thread_id << ": '" << iter->desc << "', color " << iter->color
              << ", tag " << iter->tag << ": " << iter->start_ns << " -> "
              << iter->end_ns << " ~ " << (iter->end_ns - iter->start_ns)
              << " [ns]\n";
  }
}

} // namespace rsm::impl
