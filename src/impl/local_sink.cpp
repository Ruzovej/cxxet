#include "impl/local_sink.hpp"

#include <iostream>

namespace cxxst::impl {

local_sink::local_sink(sink *aParent) noexcept : parent{aParent} {}

local_sink::~local_sink() noexcept { flush(); }

void local_sink::append_event(event::any const &evt) noexcept {
  events.append(evt);
}

void local_sink::flush() noexcept {
  if (parent) {
    parent->drain(*this);
  }
}

void local_sink::reserve(int const minimum_free_capacity) noexcept {
  events.set_default_node_capacity(minimum_free_capacity);
  events.reserve();
}

} // namespace cxxst::impl
