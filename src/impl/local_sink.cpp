#include "impl/local_sink.hpp"

namespace rsm::impl {

local_sink::local_sink(central_sink &aParent) noexcept : parent{aParent} {}

local_sink::~local_sink() noexcept { flush(); }

void local_sink::append_event(event::any const &evt) noexcept {
  events.append(evt);
}

void local_sink::flush() noexcept {
  if (!events.empty()) {
    parent.drain(events);
  }
}

void local_sink::reserve() noexcept { events.reserve(); }

} // namespace rsm::impl
