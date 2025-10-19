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

#include <string_view>

namespace cxxet_pp {

// all is thread unsafe

void set_verbose(bool const aVerbose) noexcept;

bool get_verbose() noexcept;

void log(std::string_view const msg, bool const force = false);

void log_error(std::string_view const msg);

void log_time_diff(std::string_view const msg, long long const begin,
                   long long const end, bool const force = false);

} // namespace cxxet_pp
