#pragma once

#include "impl/event/common.hpp"

namespace rsm::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.msg3086636uq

struct counter {
  common evt;
  long long timestamp_ns;
  double value;

  bool operator==(counter const &other) const noexcept {
    return evt == other.evt && timestamp_ns == other.timestamp_ns &&
           value == other.value;
  }
};

} // namespace rsm::impl::event
