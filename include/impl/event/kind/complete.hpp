#pragma once

#include "impl/event/common.hpp"

namespace rsm::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lpfof2aylapb

struct complete {
  common evt;
  long long start_ns;
  long long duration_ns;

  bool operator==(complete const &other) const noexcept {
    return evt == other.evt && start_ns == other.start_ns &&
           duration_ns == other.duration_ns;
  }
};

} // namespace rsm::impl::event
