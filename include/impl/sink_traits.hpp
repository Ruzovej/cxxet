#pragma once

#include "rsm_output_format.hpp"

namespace rsm::impl {

struct sink_traits {
  bool verbose;
  output::format target_format;
  int default_list_node_capacity;
  char const *target_filename;

  sink_traits() noexcept;

  sink_traits &set_target_filename(char const *const filename) noexcept {
    target_filename = filename;
    return *this;
  }

  sink_traits &set_target_format(output::format const fmt) noexcept {
    target_format = fmt;
    return *this;
  }

private:
  sink_traits(sink_traits const &) = delete;
  sink_traits &operator=(sink_traits const &) = delete;
  sink_traits(sink_traits &&) = delete;
  sink_traits &operator=(sink_traits &&) = delete;
};

} // namespace rsm::impl
