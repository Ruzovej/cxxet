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

  instant() = default;
  constexpr instant(char const *const aDesc, long long const aTimestamp_ns,
                    scope_t const aScope) noexcept
      : evt{aDesc}, timestamp_ns{aTimestamp_ns}, scope{aScope} {}
  constexpr instant(char const aFlag1, short const aFlag2, int const aFlag4,
                    char const *const aDesc, long long const aTimestamp_ns,
                    scope_t const aScope) noexcept
      : evt{aFlag1, aFlag2, aFlag4, aDesc},
        timestamp_ns{aTimestamp_ns}, scope{aScope} {}

  [[nodiscard]] constexpr bool operator==(instant const &other) const noexcept {
    auto const tie = [](instant const &i) {
      return std::tie(i.evt, i.timestamp_ns, i.scope);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace rsm::impl::event
