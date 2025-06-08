#include "impl/local_sink.hpp"

#include <cassert>

#include <iostream>

namespace cxxst::impl {

local_sink::local_sink(sink *aParent) noexcept : parent{aParent} {}

local_sink::~local_sink() noexcept { flush(); }

void local_sink::append_event(event::any const &evt) noexcept {
  events.safe_append(evt, default_node_capacity);
}

void local_sink::flush() noexcept {
  if (parent) {
    parent->drain(*this);
  }
}

void local_sink::reserve(int const minimum_free_capacity) noexcept {
  assert(minimum_free_capacity > 0);
  default_node_capacity = minimum_free_capacity;
  events.reserve(default_node_capacity);
}

} // namespace cxxst::impl
