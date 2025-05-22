#pragma once

#include "impl/event/kind/complete.hpp"
#include "impl/event/kind/counter.hpp"
#include "impl/event/kind/duration.hpp"
#include "impl/event/kind/instant.hpp"

namespace rsm::impl::event {

struct any {
  union {
    duration_begin begin;
    duration_end end;
    complete cmp;
    counter cnt;
    instant inst;
  } evt;

  [[nodiscard]] type get_type() const noexcept {
    return evt.begin.evt.t; // valid because of shared initial type sequence (or
                            // how is it called)
  }

  // ugh ... TODO refactor:
  bool operator==(any const &other) const noexcept {
    return get_type() == other.get_type() &&
           ((get_type() == type::duration_begin &&
             evt.begin == other.evt.begin) ||
            (get_type() == type::duration_end && evt.end == other.evt.end) ||
            (get_type() == type::complete && evt.cmp == other.evt.cmp) ||
            (get_type() == type::counter && evt.cnt == other.evt.cnt) ||
            (get_type() == type::instant && evt.inst == other.evt.inst));
  }
};

} // namespace rsm::impl::event
