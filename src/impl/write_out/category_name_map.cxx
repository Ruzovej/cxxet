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

#include "impl/write_out/category_name_map.hxx"
#include <stdexcept>

namespace cxxet::impl::write_out {

namespace {

constexpr bool is_one_bit_set(unsigned const x) noexcept {
  return x && !(x & (x - 1));
}

constexpr unsigned bit_to_index(unsigned const x) noexcept {
  unsigned idx{0};
  unsigned val{1};

  while (val != x) {
    val <<= 1;
    ++idx;
  }

  return idx;
}

} // namespace

category_name_map::category_name_map() noexcept = default;

[[nodiscard]] unsigned category_name_map::register_category_name(
    unsigned const category, std::string &&name, bool const allow_rename) {
  if (category == 0) {
    for (unsigned idx = 0; idx < max_user_categories; ++idx) {
      bool const past_the_end{idx >= names.size()};

      if (past_the_end || names[idx].empty()) {
        if (past_the_end) {
          names.resize(idx + 1);
        }
        names[idx] = std::move(name);

        return 1u << idx;
      }
    }

    throw std::runtime_error("no free category flags available");
  } else {
    if (!is_one_bit_set(category)) {
      throw std::runtime_error("category flag must have exactly one bit set");
    }

    auto const idx{bit_to_index(category)};

    if (idx >= max_user_categories) {
      throw std::runtime_error(
          "category flag uses reserved bits (highest 2 bits are "
          "reserved)");
    }

    if (names.size() <= idx) {
      names.resize(idx + 1);
    }

    if (!names[idx].empty() && !allow_rename) {
      throw std::runtime_error("category flag already registered");
    }

    names[idx] = std::move(name);

    return category;
  }
}

} // namespace cxxet::impl::write_out
