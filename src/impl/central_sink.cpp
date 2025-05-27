#include "impl/central_sink.hpp"

#include <cassert>
#include <cstdlib>

#include <iostream>

#include "impl/dump_records.hpp"
#include "impl/utils.hpp"

namespace rsm::impl {

central_sink::central_sink(bool const silent) : time_point{as_int_ns(now())} {
  events.set_default_node_capacity([] {
    // Get block_size from environment variable if available, otherwise use
    // default value
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
    std::cout << "deduced RSM_DEFAULT_BLOCK_SIZE: "
              << events.get_default_node_capacity() << '\n';
  }
}

central_sink::~central_sink() noexcept { flush(); }

void central_sink::flush() {
  std::lock_guard lck{mtx};
  if (!events.empty()) {
    try {
      if (target_filename) {
        dump_records(events, time_point, target_format, target_filename);
      }
      events.destroy();
    } catch (std::exception const &e) {
      std::cerr << "Failed to dump records: " << e.what() << '\n';
    }
  }
}

void central_sink::drain(event::list &aEvents) {
  std::lock_guard lck{mtx};
  events.drain_other(aEvents);
}

} // namespace rsm::impl
