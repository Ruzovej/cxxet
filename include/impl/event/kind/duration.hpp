#pragma once

#include "impl/event/common.hpp"

namespace rsm::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.nso4gcezn7n1

struct duration_begin {
  common evt;
  long long start_ns;

  bool operator==(duration_begin const &other) const noexcept {
    return evt == other.evt && start_ns == other.start_ns;
  }
};

struct duration_end {
  common evt;
  long long end_ns;

  bool operator==(duration_end const &other) const noexcept {
    return evt == other.evt && end_ns == other.end_ns;
  }
};

} // namespace rsm::impl::event
