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
#include <string_view>
#include <unordered_map>
#include <vector>

#include "cxxet/output/category_flag.hxx"

namespace cxxet::impl::write_out {

struct category_name_map {
  static constexpr unsigned max_user_categories{30};
  static constexpr unsigned num_categories{32};
  static_assert(sizeof(output::category_flag) * 8 == num_categories);

  static constexpr output::category_flag reserved_1{1u << 30};
  static constexpr std::string_view reserved_1_name{"cxxet_1"};
  static constexpr output::category_flag reserved_2{1u << 31};
  static constexpr std::string_view reserved_2_name{"cxxet_2"};

  category_name_map() noexcept;
  ~category_name_map() noexcept;

  [[nodiscard]] output::category_flag
  register_category_name(output::category_flag const category,
                         std::string &&name, bool const allow_rename);

  // see criteria in `cxxet/output/categories.hxx`:
  static bool is_name_valid(std::string_view const name) noexcept;
  bool is_name_used(std::string_view const name) const noexcept;

  std::string_view
  get_joined_category_names(output::category_flag const category_bits) const;

private:
  // better than `std::array` - to save (stack) space when not used ...
  std::vector<std::string> names{};
  mutable std::unordered_map<output::category_flag, std::string,
                             output::category_flag::hasher>
      built_names;
};

} // namespace cxxet::impl::write_out
