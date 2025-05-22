#pragma once

#include "impl/sink.hpp"

namespace rsm::impl {

struct local_sink : sink {
  explicit local_sink(sink *aParent);
  ~local_sink() noexcept override;

  inline void append_event(event::any const &evt) noexcept {
    events.append(evt);
  }

  void flush_to_parent();

  local_sink &set_parent_sink(sink *aParent) noexcept;

  local_sink &set_default_list_node_capacity(
      int const default_list_node_capacity = 64) noexcept;

  local_sink &reserve();

private:
  sink *parent{nullptr};
};

} // namespace rsm::impl
