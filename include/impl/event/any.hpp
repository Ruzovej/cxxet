#pragma once

#include <new>
#include <utility>

#include "impl/event/kind/complete.hpp"
#include "impl/event/kind/counter.hpp"
#include "impl/event/kind/duration.hpp"
#include "impl/event/kind/instant.hpp"

namespace rsm::impl::event {

struct any {
  union {
    struct {
      common<> c;
    } common_base;
    duration_begin dur_begin;
    duration_end dur_end;
    complete cmpl;
    counter cntr;
    instant inst;
  } evt;

  any() noexcept : evt{} {}

  [[nodiscard]] constexpr type_t get_type() const noexcept {
    // valid because of `common initial sequence of members`:
    return evt.common_base.c.type;
  }

  // ugh ... TODO refactor those below - they wouldn't scale well & basically
  // are a lot of repetition:
  template <typename EventType, typename = std::enable_if_t<
                                    std::is_same_v<EventType, duration_begin> ||
                                    std::is_same_v<EventType, duration_end> ||
                                    std::is_same_v<EventType, complete> ||
                                    std::is_same_v<EventType, counter> ||
                                    std::is_same_v<EventType, instant>>>
  any(EventType const &e) noexcept : evt{} {
    if constexpr (std::is_same_v<EventType, duration_begin>) {
      new (&evt.dur_begin) duration_begin{e};
    } else if constexpr (std::is_same_v<EventType, duration_end>) {
      new (&evt.dur_end) duration_end{e};
    } else if constexpr (std::is_same_v<EventType, complete>) {
      new (&evt.cmpl) complete{e};
    } else if constexpr (std::is_same_v<EventType, counter>) {
      new (&evt.cntr) counter{e};
    } else if constexpr (std::is_same_v<EventType, instant>) {
      new (&evt.inst) instant{e};
    }
  }

  [[nodiscard]] constexpr bool operator==(any const &other) const noexcept {
    return get_type() == other.get_type() && get_type() != type_t::unknown &&
           ((get_type() == type_t::duration_begin &&
             evt.dur_begin == other.evt.dur_begin) ||
            (get_type() == type_t::duration_end &&
             evt.dur_end == other.evt.dur_end) ||
            (get_type() == type_t::complete && evt.cmpl == other.evt.cmpl) ||
            (get_type() == type_t::counter && evt.cntr == other.evt.cntr) ||
            (get_type() == type_t::instant && evt.inst == other.evt.inst));
  }
};

} // namespace rsm::impl::event
