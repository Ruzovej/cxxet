#pragma once

#include <new>
#include <utility>

#include "impl/event/common.hxx"
#include "impl/event/kind/complete.hxx"
#include "impl/event/kind/counter.hxx"
#include "impl/event/kind/duration.hxx"
#include "impl/event/kind/instant.hxx"

namespace cxxst::impl::event {

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

  // valid because of `common initial sequence of members`:
  [[nodiscard]] constexpr type_t get_type() const noexcept {
    return evt.common_base.c.type;
  }

  [[nodiscard]] constexpr char const *get_name() const noexcept {
    return get_type() == event::type_t::counter ? "Counter"
           : get_type() == event::type_t::duration_end
               // TODO maybe don't return empty string, and rather skip writing
               // out `"name":"",` completely (in `dump_records`)...:
               ? (evt.common_base.c.desc ? evt.common_base.c.desc : "")
               : evt.common_base.c.desc;
  }

  [[nodiscard]] constexpr auto get_ph() const noexcept {
    return static_cast<std::underlying_type_t<event::type_t>>(get_type());
  }

  // ugh ... TODO refactor those below - they wouldn't scale well & basically
  // are a lot of repetition:
  template <typename EventType, typename = std::enable_if_t<
                                    std::is_same_v<EventType, duration_begin> ||
                                    std::is_same_v<EventType, duration_end> ||
                                    std::is_same_v<EventType, complete> ||
                                    std::is_same_v<EventType, counter> ||
                                    std::is_same_v<EventType, instant>>>
  any(EventType const &e) noexcept : any{} {
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

  // used only for testing - maybe hide it otherwise (via #if-def, etc.)?
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

} // namespace cxxst::impl::event
