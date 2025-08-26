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

#include "impl/event/common.hxx"

namespace cxxet::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.nso4gcezn7n1

struct duration_begin {
  static constexpr trace_type t{trace_type::duration_begin};

  common<t> evt;
  long long start_ns;

  constexpr duration_begin(unsigned const aCategories, char const *const aDesc,
                           long long const aStart_ns) noexcept
      : evt{aCategories, aDesc}, start_ns{aStart_ns} {}

#ifdef CXXET_WITH_UNIT_TESTS
  constexpr duration_begin(char const aFlag1, short const aFlag2,
                           unsigned const aCategories, char const *const aDesc,
                           long long const aStart_ns) noexcept
      : evt{aFlag1, aFlag2, aCategories, aDesc}, start_ns{aStart_ns} {}

  constexpr bool operator==(duration_begin const &other) const noexcept {
    auto const tie = [](duration_begin const &db) {
      return std::tie(db.evt, db.start_ns);
    };
    return tie(*this) == tie(other);
  }
#endif
};

struct duration_end {
  static constexpr trace_type t{trace_type::duration_end};

  common<t> evt;
  long long end_ns;

  constexpr duration_end(unsigned const aCategories, char const *const aDesc,
                         long long const aEnd_ns) noexcept
      : evt{aCategories, aDesc}, end_ns{aEnd_ns} {}

#ifdef CXXET_WITH_UNIT_TESTS
  constexpr duration_end(char const aFlag1, short const aFlag2,
                         unsigned const aCategories, char const *const aDesc,
                         long long const aEnd_ns) noexcept
      : evt{aFlag1, aFlag2, aCategories, aDesc}, end_ns{aEnd_ns} {}

  [[nodiscard]] constexpr bool
  operator==(duration_end const &other) const noexcept {
    auto const tie = [](duration_end const &de) {
      return std::tie(de.evt, de.end_ns);
    };
    return tie(*this) == tie(other);
  }
#endif
};

} // namespace cxxet::impl::event
