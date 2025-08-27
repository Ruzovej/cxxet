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

#include <utility>

#include "cxxet/macros/linkage.h"
#include "cxxet/output/category_flag.hxx"
#include "cxxet/timepoint.hxx"

namespace cxxet::mark {

CXXET_IMPL_API void submit_counter(output::category_flag const categories,
                                   char const *const name,
                                   long long const timestamp_ns,
                                   double const value) noexcept;

inline void do_submit_counter(output::category_flag const categories,
                              char const *const name,
                              double const value) noexcept {
  auto const now_ns{impl::as_int_ns(impl::now())};
  submit_counter(categories, name, now_ns, value);
}

inline void do_submit_counter(char const *const name,
                              double const value) noexcept {
  do_submit_counter(output::category_flag_none, name, value);
}

template <typename... Args>
void do_submit_counters_impl(long long const timestamp_ns,
                             output::category_flag const categories,
                             char const *const name, double const value,
                             Args &&...args) noexcept {
  submit_counter(categories, name, timestamp_ns, value);
  if constexpr (sizeof...(args) > 0) {
    do_submit_counters_impl(timestamp_ns, categories,
                            std::forward<Args>(args)...);
  }
}

template <typename... Args>
void do_submit_counters(output::category_flag const categories,
                        char const *const name, double const value,
                        Args &&...args) noexcept {
  static_assert(sizeof...(args) % 2 == 0, "Uneven number of arguments");
  static_assert(
      sizeof...(args) >= 2,
      "Submitting only single counter - use `do_submit_counter` instead");

  auto const now_ns{impl::as_int_ns(impl::now())};
  do_submit_counters_impl(now_ns, categories, name, value,
                          std::forward<Args>(args)...);
}

template <typename... Args>
void do_submit_counters(char const *const name, double const value,
                        Args &&...args) noexcept {
  do_submit_counters(output::category_flag_none, name, value,
                     std::forward<Args>(args)...);
}

} // namespace cxxet::mark
