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

namespace cxxet::impl::sink {

struct properties {
  long long const time_point_zero_ns;
  bool verbose;
  int default_list_node_capacity;
  char const *default_target_filename;

  [[nodiscard]] static properties const &instance() noexcept;

#ifndef CXXET_WITH_UNIT_TESTS
private:
#endif
  properties() noexcept;

private:
  properties(properties const &) = delete;
  properties &operator=(properties const &) = delete;
  properties(properties &&) = delete;
  properties &operator=(properties &&) = delete;
};

} // namespace cxxet::impl::sink
