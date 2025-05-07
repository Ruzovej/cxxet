#include "impl/global.hpp"

#include <cstdlib>

#include <iostream>
#include <mutex>

namespace rsm::impl {

global::global()
    : block_size([] {
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

[[nodiscard]] global *global::instance() noexcept {
  static global g;
  return &g;
}

namespace {
std::mutex global_access_mutex{};
}

void global::append(records *recs) noexcept {
  std::lock_guard lck{global_access_mutex};
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

void global::print_records() const {
  for (auto iter{first}; iter != nullptr; iter = iter->next) {
    iter->print_records();
  }
}

global::~global() noexcept {
  while (first) {
    auto const next{first->next};
    delete first;
    first = next;
  }
}

} // namespace rsm::impl
