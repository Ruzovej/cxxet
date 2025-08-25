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

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.msg3086636uq

struct counter {
  static constexpr type_t t{type_t::counter};

  common<t> evt;
  long long timestamp_ns;
  double value;

  counter() = default;
  constexpr counter(char const *const name, long long const aTimestamp_ns,
                    double const aValue) noexcept
      : evt{name}, timestamp_ns{aTimestamp_ns}, value{aValue} {}
  constexpr counter(unsigned const aCategories, char const *const name,
                    long long const aTimestamp_ns, double const aValue) noexcept
      : evt{aCategories, name}, timestamp_ns{aTimestamp_ns}, value{aValue} {}

  [[nodiscard]] constexpr char const *get_quantity_name() const noexcept {
    return evt.desc;
  }

#ifdef CXXET_WITH_UNIT_TESTS
  constexpr counter(char const aFlag1, short const aFlag2,
                    unsigned const aCategories, char const *const name,
                    long long const aTimestamp_ns, double const aValue) noexcept
      : evt{aFlag1, aFlag2, aCategories, name}, timestamp_ns{aTimestamp_ns},
        value{aValue} {}

  [[nodiscard]] constexpr bool operator==(counter const &other) const noexcept {
    auto const tie = [](counter const &c) {
      return std::tie(c.evt, c.timestamp_ns, c.value);
    };
    return tie(*this) == tie(other);
  }
#endif
};

} // namespace cxxet::impl::event
