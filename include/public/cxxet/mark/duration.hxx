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

#include "cxxet/mark/duration_begin.hxx"
#include "cxxet/mark/duration_end.hxx"
#include "cxxet/output/category_flag.hxx"

namespace cxxet::mark {

struct duration {
  inline duration(output::category_flag const categories,
                  char const *const desc) noexcept {
    duration_begin(categories, desc);
  }
  inline explicit duration(char const *const desc) noexcept
      : duration{output::category_flag_none, desc} {}

  inline ~duration() noexcept { duration_end(); }

private:
  duration(duration const &) = delete;
  duration &operator=(duration const &) = delete;
  duration(duration &&) = delete;
  duration &operator=(duration &&) = delete;
};

} // namespace cxxet::mark
