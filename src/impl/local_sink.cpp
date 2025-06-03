#include "impl/local_sink.hpp"

namespace cxxst::impl {

local_sink::local_sink(central_sink &aParent) noexcept : parent{aParent} {
  events.set_default_node_capacity(
      parent.get_traits().default_list_node_capacity);
}

local_sink::~local_sink() noexcept { flush(); }

void local_sink::append_event(event::any const &evt) noexcept {
  events.append(evt);
}

void local_sink::flush() noexcept {
  if (!events.empty()) {
    parent.drain(events);
  }
}

void local_sink::reserve(int const minimum_free_capacity) noexcept {
  events.set_default_node_capacity(
      minimum_free_capacity <= 0
          ? parent.get_traits().default_list_node_capacity
          : minimum_free_capacity);
  events.reserve();
}

} // namespace cxxst::impl
