#include "impl/local_sink.hpp"

namespace rsm::impl {

local_sink::local_sink(central_sink &aParent) : parent{aParent} {}

local_sink::~local_sink() noexcept { flush(); }

void local_sink::flush() {
  if (!events.empty()) {
    parent.drain(events);
  }
}

void local_sink::reserve() { events.reserve(); }

} // namespace rsm::impl
