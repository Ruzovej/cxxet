#include "impl/marker_sink.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "impl/dump_records.hpp"
#include "impl/utils.hpp"

namespace rsm::impl {

// Global instance singleton
[[nodiscard]] marker_sink* marker_sink::global_instance() noexcept {
  static marker_sink global_sink(nullptr);
  return &global_sink;
}

// Thread-local instance accessor
[[nodiscard]] marker_sink* marker_sink::thread_instance() noexcept {
  thread_local marker_sink thread_sink(global_instance());
  return &thread_sink;
}

// Initialize the thread-local sink
void marker_sink::init_thread() {
  [[maybe_unused]] auto const global_inst{
      global_instance()}; // ensure global is initialized before (and hence
                          // destroyed after) any thread_local instance
  auto const inst{thread_instance()};
  
  // Just allocate the first records block if needed
  if (!inst->first) {
    inst->allocate_next_records();
  }
}

// Constructor with different initialization based on whether it's a global or thread-local instance
marker_sink::marker_sink(marker_sink* parent)
    : parent_sink(parent),
      time_point(parent ? 0 : as_int_ns(now())),
      block_size(parent ? parent->get_block_size() : []() {
        // Get block_size from environment variable if available, otherwise use default value of 64
        const char* env_block_size = std::getenv("RSM_DEFAULT_BLOCK_SIZE");
        if (env_block_size && *env_block_size != '\0') {
          try {
            return static_cast<unsigned>(std::stoul(env_block_size));
          } catch (...) {
            // In case of invalid conversion, keep the default value
          }
        }
        return 64u;
      }()) {
  // Only print this message when constructing the global instance
  if (!parent) {
    std::cout << "deduced RSM_DEFAULT_BLOCK_SIZE: " << block_size << '\n';
  }
}

// Destructor that flushes to parent if needed
marker_sink::~marker_sink() noexcept {
  if (parent_sink) {
    flush_to_parent();
  } else {
    deallocate_current();
  }
}

// Allocate a new records block
void marker_sink::allocate_next_records() {
  auto target{first ? &last->next : &last};
  *target = new records(get_block_size());
  if (!first) {
    first = *target;
  } else {
    last = *target;
  }
}

// Append records from another sink
void marker_sink::append(records* recs) noexcept {
  if (parent_sink) {
    // Thread-local sinks should just pass to parent
    parent_sink->append(recs);
  } else {
    // Global sink needs to lock and manage the linked list
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
}

// Flush records to parent sink
void marker_sink::flush_to_parent() noexcept {
  if (parent_sink && first) {
    parent_sink->append(first);
    first = last = nullptr;
  }
}

// Dump records and clean up
void marker_sink::dump_and_deallocate_collected_records(output::format const fmt,
                                                      char const* const filename) {
  // This should only be called on the global instance
  assert(!parent_sink && "dump_and_deallocate_collected_records should only be called on the global instance");
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

// Deallocate all records
void marker_sink::deallocate_current() noexcept {
  while (first) {
    auto const next{first->next};
    delete first;
    first = next;
  }
  last = nullptr;
}

} // namespace rsm::impl