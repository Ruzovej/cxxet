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
#include "cxxet/output/category_flag.hxx"

namespace cxxet::mark {

CXXET_IMPL_API void process_name(output::category_flag const categories,
                                 char const *const proc_name) noexcept;

inline void process_name(char const *const proc_name) noexcept {
  process_name(output::category_flag_none, proc_name);
}

CXXET_IMPL_API void process_label(output::category_flag const categories,
                                  char const *const proc_label) noexcept;

inline void process_label(char const *const proc_label) noexcept {
  process_label(output::category_flag_none, proc_label);
}

CXXET_IMPL_API void process_sort_index(output::category_flag const categories,
                                       int const proc_sort_index) noexcept;

inline void process_sort_index(int const proc_sort_index) noexcept {
  process_sort_index(output::category_flag_none, proc_sort_index);
}

CXXET_IMPL_API void thread_name(output::category_flag const categories,
                                char const *const th_name) noexcept;

inline void thread_name(char const *const th_name) noexcept {
  thread_name(output::category_flag_none, th_name);
}

CXXET_IMPL_API void thread_sort_index(output::category_flag const categories,
                                      int const th_sort_index) noexcept;

inline void thread_sort_index(int const th_sort_index) noexcept {
  thread_sort_index(output::category_flag_none, th_sort_index);
}

} // namespace cxxet::mark
