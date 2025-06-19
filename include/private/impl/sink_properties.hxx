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

#include "cxxet/output_format.hxx"

namespace cxxet::impl {

struct sink_properties {
  long long const time_point_zero_ns;
  bool verbose;
  output::format default_target_format;
  int default_list_node_capacity;
  char const *default_target_filename;

  sink_properties() noexcept;

#ifdef CXXET_WITH_UNIT_TESTS
  sink_properties &set_target_filename(char const *const filename) noexcept {
    default_target_filename = filename;
    return *this;
  }
#endif

private:
  sink_properties(sink_properties const &) = delete;
  sink_properties &operator=(sink_properties const &) = delete;
  sink_properties(sink_properties &&) = delete;
  sink_properties &operator=(sink_properties &&) = delete;
};

} // namespace cxxet::impl
