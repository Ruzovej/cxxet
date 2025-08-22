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

#include "cxxet/macros/linkage.h"

namespace cxxet::mark {

CXXET_IMPL_API void set_process_name(char const *const proc_name) noexcept;

CXXET_IMPL_API void set_process_label(char const *const proc_label) noexcept;

CXXET_IMPL_API void set_process_sort_index(int const proc_sort_index) noexcept;

CXXET_IMPL_API void set_thread_name(char const *const thread_name) noexcept;

CXXET_IMPL_API void set_thread_sort_index(int const thread_sort_index) noexcept;

} // namespace cxxet::mark
