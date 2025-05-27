#pragma once

#include "rsm_output_format.hpp"

namespace rsm::impl {

struct sink_properties {
  bool verbose;
  output::format target_format;
  int default_list_node_capacity;
  char const *target_filename;

  sink_properties() noexcept;

  sink_properties &set_target_filename(char const *const filename) noexcept {
    target_filename = filename;
    return *this;
  }

  sink_properties &set_target_format(output::format const fmt) noexcept {
    target_format = fmt;
    return *this;
  }

private:
  sink_properties(sink_properties const &) = delete;
  sink_properties &operator=(sink_properties const &) = delete;
  sink_properties(sink_properties &&) = delete;
  sink_properties &operator=(sink_properties &&) = delete;
};

} // namespace rsm::impl
