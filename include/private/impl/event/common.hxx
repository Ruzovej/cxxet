/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <tuple>

namespace cxxet::impl::event {

// inspired by
// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU

enum class type_t : char {
  duration_begin = 'B',
  duration_end = 'E',
  complete = 'X',
  instant = 'i',
  counter = 'C',
  // TODO implement or at least consider:
  // async: 'b' nestable begin, 'n' nestable instant, 'e' nestable end
  // flow: 's' start, 't' step, 'f' end
  unknown = '\0',
};

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.uxpopqvbjezh
template <type_t bound_type = type_t::unknown> struct common {
  //                        // related to field:
  type_t const type{bound_type}; // "ph"
  char flag_1;                   // explicit padding - unspecified meaning
  short flag_2;                  // explicit padding - unspecified meaning
  int flag_4;                    // explicit padding - unspecified meaning
  const char *desc;              // "name"
  // optional (or not?!) fields:
  // * "cat" -> TODO have it here or not?!
  // other mandatory fields:
  // * "pid", "tid" -> provided by the sink, etc.
  // * "ts", "args", ... -> provided by the specific event type

  constexpr common() = default;
  constexpr common(char const *const aDesc) noexcept
      : flag_1{0}, flag_2{0}, flag_4{0}, desc{aDesc} {}
  constexpr common(char const aFlag1, short const aFlag2, int const aFlag4,
                   char const *const aDesc) noexcept
      : flag_1{aFlag1}, flag_2{aFlag2}, flag_4{aFlag4}, desc{aDesc} {}

  [[nodiscard]] constexpr bool operator==(common const &other) const noexcept {
    auto const tie = [](common const &c) {
      return std::tie(c.type, c.flag_1, c.flag_2, c.flag_4, c.desc);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace cxxet::impl::event
