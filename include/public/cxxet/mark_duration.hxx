/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "cxxet/mark_duration_begin.hxx"
#include "cxxet/mark_duration_end.hxx"

namespace cxxet {

struct mark_duration {
  inline mark_duration(char const *desc) noexcept { mark_duration_begin(desc); }

  inline ~mark_duration() noexcept { mark_duration_end(); }

private:
  mark_duration(mark_duration const &) = delete;
  mark_duration &operator=(mark_duration const &) = delete;
  mark_duration(mark_duration &&) = delete;
  mark_duration &operator=(mark_duration &&) = delete;
};

} // namespace cxxet
