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

#include <string>

#include "cxxet/macros/linkage.h"

namespace cxxet::output {

// * if the category is invalid (must have exactly one bit set, with 2 highest
// reserved for internal use), or already registered (if `allow_rename` is
// false), throws `std::runtime_error`;
//
// * if the input is zero, return next free category flag
//
// * returns the category flag itself (either input param. `category` itself, or
// the "deduced" next one)
[[nodiscard]] unsigned CXXET_IMPL_API register_category_name(
    unsigned const category, std::string name, bool const allow_rename = false);

[[nodiscard]] inline unsigned
register_category_name(std::string name, bool const allow_rename = false) {
  return register_category_name(0, std::move(name), allow_rename);
}

} // namespace cxxet::output
