#include "impl/local_sink.hpp"

#include <cassert>

#include "impl/central_sink.hpp"

namespace rsm::impl {

local_sink::local_sink(sink *aParent) : sink{}, parent{aParent} {}

local_sink::~local_sink() noexcept { flush_to_parent(); }

void local_sink::flush_to_parent() {
  if (parent && !events.empty()) {
    parent->drain(*this);
  }
}

local_sink &local_sink::set_parent_sink(sink *aParent) noexcept {
  assert((parent && events.empty()) &&
         "attempting to set a parent sink with unflushed records");
  parent = aParent;
  return *this;
}

local_sink &local_sink::set_default_list_node_capacity(
    int const default_list_node_capacity) noexcept {
  events.set_default_node_capacity(default_list_node_capacity);
  return *this;
}

local_sink &local_sink::reserve() {
  events.reserve();
  return *this;
}

} // namespace rsm::impl
