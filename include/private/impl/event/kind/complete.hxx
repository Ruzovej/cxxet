#pragma once

#include "impl/event/common.hxx"

namespace cxxst::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lpfof2aylapb

struct complete {
  static constexpr type_t t{type_t::complete};

  common<t> evt;
  long long start_ns;
  long long duration_ns;

  complete() = default;
  constexpr complete(char const *const aDesc, long long const aStart_ns,
                     long long const aDuration_ns) noexcept
      : evt{aDesc}, start_ns{aStart_ns}, duration_ns{aDuration_ns} {}
  constexpr complete(char const aFlag1, short const aFlag2, int const aFlag4,
                     char const *const aDesc, long long const aStart_ns,
                     long long const aDuration_ns) noexcept
      : evt{aFlag1, aFlag2, aFlag4, aDesc}, start_ns{aStart_ns},
        duration_ns{aDuration_ns} {}

  [[nodiscard]] constexpr bool
  operator==(complete const &other) const noexcept {
    auto const tie = [](complete const &c) {
      return std::tie(c.evt, c.start_ns, c.duration_ns);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace cxxst::impl::event
