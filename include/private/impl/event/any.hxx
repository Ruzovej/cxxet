/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License along
  with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <new>
#include <utility>

#include "impl/event/common.hxx"
#include "impl/event/kind/complete.hxx"
#include "impl/event/kind/counter.hxx"
#include "impl/event/kind/duration.hxx"
#include "impl/event/kind/instant.hxx"
#include "impl/event/kind/metadata.hxx"

namespace cxxet::impl::event {

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
    metadata meta;
  } evt;

  any() noexcept : evt{} {}

  // valid because of `common initial sequence of members`:
  [[nodiscard]] constexpr type_t get_type() const noexcept {
    return evt.common_base.c.type;
  }

  [[nodiscard]] constexpr char const *get_name() const noexcept {
    return get_type() == event::type_t::metadata  ? evt.meta.get_name()
           : get_type() == event::type_t::counter ? "Counter"
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
                                    std::is_same_v<EventType, instant> ||
                                    std::is_same_v<EventType, metadata>>>
  any(EventType const &e) noexcept : any{} {
    if constexpr (std::is_same_v<EventType, duration_begin>) {
      new (&evt.dur_begin) EventType{e};
    } else if constexpr (std::is_same_v<EventType, duration_end>) {
      new (&evt.dur_end) EventType{e};
    } else if constexpr (std::is_same_v<EventType, complete>) {
      new (&evt.cmpl) EventType{e};
    } else if constexpr (std::is_same_v<EventType, counter>) {
      new (&evt.cntr) EventType{e};
    } else if constexpr (std::is_same_v<EventType, instant>) {
      new (&evt.inst) EventType{e};
    } else if constexpr (std::is_same_v<EventType, metadata>) {
      new (&evt.meta) EventType{e};
    }
  }

#ifdef CXXET_WITH_UNIT_TESTS
  [[nodiscard]] constexpr bool operator==(any const &other) const noexcept {
    return get_type() == other.get_type() && get_type() != type_t::unknown &&
           ((get_type() == type_t::duration_begin &&
             evt.dur_begin == other.evt.dur_begin) ||
            (get_type() == type_t::duration_end &&
             evt.dur_end == other.evt.dur_end) ||
            (get_type() == type_t::complete && evt.cmpl == other.evt.cmpl) ||
            (get_type() == type_t::counter && evt.cntr == other.evt.cntr) ||
            (get_type() == type_t::instant && evt.inst == other.evt.inst) ||
            (get_type() == type_t::metadata && evt.meta == other.evt.meta));
  }
#endif
};

} // namespace cxxet::impl::event
