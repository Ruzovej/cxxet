#pragma once

#include "impl/event/common.hpp"

namespace rsm::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lenwiilchoxp

struct instant {
  static constexpr type_t t{type_t::instant};

  enum class scope_t : char {
    global = 'g',
    process = 'p',
    thread = 't',
  };

  common<t> evt;
  long long timestamp_ns;
  scope_t scope;
  char i_flag_1;  // explicit padding - unspecified meaning
  short i_flag_2; // explicit padding - unspecified meaning
  int i_flag_4;   // explicit padding - unspecified meaning

  instant() = default;
  constexpr instant(char const *const aDesc, long long const aTimestamp_ns,
                    scope_t const aScope) noexcept
      : evt{aDesc}, timestamp_ns{aTimestamp_ns}, scope{aScope}, i_flag_1{0},
        i_flag_2{0}, i_flag_4{0} {}
  constexpr instant(char const aFlag1, short const aFlag2, int const aFlag4,
                    char const *const aDesc, long long const aTimestamp_ns,
                    scope_t const aScope) noexcept
      : evt{aFlag1, aFlag2, aFlag4, aDesc}, timestamp_ns{aTimestamp_ns},
        scope{aScope}, i_flag_1{0}, i_flag_2{0}, i_flag_4{0} {}
  constexpr instant(char const aFlag1, short const aFlag2, int const aFlag4,
                    char const *const aDesc, long long const aTimestamp_ns,
                    scope_t const aScope, char const aI_flag1,
                    short const aI_flag2, int const aI_flag4) noexcept
      : evt{aFlag1, aFlag2, aFlag4, aDesc}, timestamp_ns{aTimestamp_ns},
        scope{aScope}, i_flag_1{aI_flag1}, i_flag_2{aI_flag2}, i_flag_4{
                                                                   aI_flag4} {}

  [[nodiscard]] constexpr bool operator==(instant const &other) const noexcept {
    auto const tie = [](instant const &i) {
      return std::tie(i.evt, i.timestamp_ns, i.scope, i.i_flag_1, i.i_flag_2,
                      i.i_flag_4);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace rsm::impl::event
