#include "impl/global.hpp"

#include <cstdlib>

#include <iostream>

#include "impl/dump_records.hpp"
#include "impl/utils.hpp"

namespace rsm::impl {

[[nodiscard]] global *global::instance() noexcept {
  static global g;
  return &g;
}

void global::append(records *recs) noexcept {
  std::lock_guard lck{mtx};
  if (!first) {
    first = recs;
  } else {
    last->next = recs;
  }
  while (recs->next) {
    recs = recs->next;
  }
  last = recs;
}

void global::dump_and_deallocate_collected_records(output::format const fmt,
                                                   char const *const filename) {
  std::lock_guard lck{mtx};
  if (fmt == output::format::raw_naive_v0) {
    for (auto iter{first}; iter != nullptr; iter = iter->next) {
      iter->print_records();
    }
  } else {
    dump_records(first, time_point, fmt, filename);
  }
  deallocate_current();
}

global::global()
    : time_point{as_int_ns(now())}, block_size([] {
        // Get block_size from environment variable if available, otherwise use
        // default value of 64
        const char *env_block_size = std::getenv("RSM_DEFAULT_BLOCK_SIZE");
        if (env_block_size && *env_block_size != '\0') {
          try {
            return static_cast<unsigned>(std::stoul(env_block_size));
          } catch (...) {
            // In case of invalid conversion, keep the default value
          }
        }
        return 64u;
      }()) {
  std::cout << "deduced RSM_DEFAULT_BLOCK_SIZE: " << block_size << '\n';
}

global::~global() noexcept { deallocate_current(); }

void global::deallocate_current() noexcept {
  while (first) {
    auto const next{first->next};
    delete first;
    first = next;
  }
}

} // namespace rsm::impl
