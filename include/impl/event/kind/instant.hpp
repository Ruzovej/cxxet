#pragma once

#include "impl/event/common.hpp"

namespace rsm::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lenwiilchoxp

struct instant {
  common evt;
  long long timestamp_ns;
  
  bool operator==(instant const &other) const noexcept {
    return evt == other.evt && timestamp_ns == other.timestamp_ns;
  }
};

} // namespace rsm::impl::event
