#include "impl/central_sink.hpp"

#include <cassert>
#include <cstdlib>

#include <iostream>

#include "impl/dump_records.hpp"
#include "impl/utils.hpp"

namespace rsm::impl {

central_sink::central_sink(bool const silent)
    : sink{}, time_point{as_int_ns(now())} {
  set_default_list_node_capacity([] {
    // Get block_size from environment variable if available, otherwise use
    // default value of 64
    const char *env_block_size = std::getenv("RSM_DEFAULT_BLOCK_SIZE");
    if (env_block_size && *env_block_size != '\0') {
      try {
        return static_cast<int>(std::stoul(env_block_size));
      } catch (...) {
        // In case of invalid conversion, keep the default value
      }
    }
    return 64;
  }());
  if (!silent) {
    std::cout << "deduced RSM_DEFAULT_BLOCK_SIZE: " << get_default_capacity()
              << '\n';
  }
}

central_sink::~central_sink() noexcept { flush(); }

void central_sink::flush() {
  std::lock_guard lck{mtx};
  if (!events.empty()) {
    if (target_filename) {
      dump_records(events, time_point, target_format, target_filename);
    }
    events.destroy();
  }
}

void central_sink::drain(sink &other) {
  std::lock_guard lck{mtx};
  sink::drain(other);
}

} // namespace rsm::impl
