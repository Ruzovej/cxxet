#pragma once

#include "cxxst/scope.hpp"
#include "impl/event/common.hpp"

namespace cxxst::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lenwiilchoxp

struct instant {
  static constexpr type_t t{type_t::instant};

  common<t> evt;
  scope_t scope;
  char i_flag_1;  // explicit padding - unspecified meaning
  short i_flag_2; // explicit padding - unspecified meaning
  int i_flag_4;   // explicit padding - unspecified meaning
  long long timestamp_ns;

  instant() = default;
  constexpr instant(char const *const aDesc, scope_t const aScope,
                    long long const aTimestamp_ns) noexcept
      : evt{aDesc}, scope{aScope}, i_flag_1{0}, i_flag_2{0}, i_flag_4{0},
        timestamp_ns{aTimestamp_ns} {}
  constexpr instant(char const aFlag1, short const aFlag2, int const aFlag4,
                    char const *const aDesc, scope_t const aScope,
                    long long const aTimestamp_ns) noexcept
      : evt{aFlag1, aFlag2, aFlag4, aDesc}, scope{aScope}, i_flag_1{0},
        i_flag_2{0}, i_flag_4{0}, timestamp_ns{aTimestamp_ns} {}
  constexpr instant(char const aFlag1, short const aFlag2, int const aFlag4,
                    char const *const aDesc, scope_t const aScope,
                    char const aI_flag1, short const aI_flag2,
                    int const aI_flag4, long long const aTimestamp_ns) noexcept
      : evt{aFlag1, aFlag2, aFlag4, aDesc}, scope{aScope}, i_flag_1{aI_flag1},
        i_flag_2{aI_flag2}, i_flag_4{aI_flag4}, timestamp_ns{aTimestamp_ns} {}

  [[nodiscard]] constexpr bool operator==(instant const &other) const noexcept {
    auto const tie = [](instant const &i) {
      return std::tie(i.evt, i.scope, i.i_flag_1, i.i_flag_2, i.i_flag_4,
                      i.timestamp_ns);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace cxxst::impl::event
