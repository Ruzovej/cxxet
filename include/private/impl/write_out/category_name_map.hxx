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
#include <vector>

namespace cxxet::impl::write_out {

struct category_name_map {
  static constexpr unsigned max_user_categories{30};
  static constexpr unsigned num_categories{31};

  category_name_map() noexcept;

  [[nodiscard]] unsigned register_category_name(unsigned const category,
                                                std::string &&name,
                                                bool const allow_rename);

private:
  std::vector<std::string> names{}; // to save stack space when not used ...
};

} // namespace cxxet::impl::write_out
