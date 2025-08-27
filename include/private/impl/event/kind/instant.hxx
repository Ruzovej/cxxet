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

#include "cxxet/scope.hxx"
#include "impl/event/common.hxx"

namespace cxxet::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lenwiilchoxp

struct instant {
  static constexpr trace_type t{trace_type::instant};

  common<t> evt;
  scope_t scope;
  char i_flag_1;  // explicit padding - unspecified meaning
  short i_flag_2; // explicit padding - unspecified meaning
  int i_flag_4;   // explicit padding - unspecified meaning
  long long timestamp_ns;

  constexpr instant(output::category_flag const aCategories,
                    char const *const aDesc, scope_t const aScope,
                    long long const aTimestamp_ns) noexcept
      : evt{aCategories, aDesc}, scope{aScope}, i_flag_1{0}, i_flag_2{0},
        i_flag_4{0}, timestamp_ns{aTimestamp_ns} {}

#ifdef CXXET_WITH_UNIT_TESTS
  constexpr instant(char const aFlag1, short const aFlag2,
                    output::category_flag const aCategories,
                    char const *const aDesc, scope_t const aScope,
                    char const aI_flag1, short const aI_flag2,
                    int const aI_flag4, long long const aTimestamp_ns) noexcept
      : evt{aFlag1, aFlag2, aCategories, aDesc}, scope{aScope},
        i_flag_1{aI_flag1}, i_flag_2{aI_flag2}, i_flag_4{aI_flag4},
        timestamp_ns{aTimestamp_ns} {}

  [[nodiscard]] constexpr bool operator==(instant const &other) const noexcept {
    auto const tie = [](instant const &i) {
      return std::tie(i.evt, i.scope, i.i_flag_1, i.i_flag_2, i.i_flag_4,
                      i.timestamp_ns);
    };
    return tie(*this) == tie(other);
  }
#endif
};

} // namespace cxxet::impl::event
