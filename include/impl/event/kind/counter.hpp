#pragma once

#include "impl/event/common.hpp"

namespace rsm::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.msg3086636uq

struct counter {
  static constexpr type_t t{type_t::counter};

  common<t> evt;
  long long timestamp_ns;
  double value;

  counter() = default;
  constexpr counter(char const *const name, long long const aTimestamp_ns,
                    double const aValue) noexcept
      : evt{name}, timestamp_ns{aTimestamp_ns}, value{aValue} {}
  constexpr counter(char const aFlag1, short const aFlag2, int const aFlag4,
                    char const *const name, long long const aTimestamp_ns,
                    double const aValue) noexcept
      : evt{aFlag1, aFlag2, aFlag4, name},
        timestamp_ns{aTimestamp_ns}, value{aValue} {}

  [[nodiscard]] constexpr bool operator==(counter const &other) const noexcept {
    auto const tie = [](counter const &c) {
      return std::tie(c.evt, c.timestamp_ns, c.value);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace rsm::impl::event
