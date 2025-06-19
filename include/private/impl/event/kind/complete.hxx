/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "impl/event/common.hxx"

namespace cxxet::impl::event {

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

} // namespace cxxet::impl::event
