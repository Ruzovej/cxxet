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
#include "cxxet/timepoint.hxx"

namespace cxxet::mark {

CXXET_IMPL_API void submit_counter(char const *const name,
                                   long long const timestamp_ns,
                                   double const value) noexcept;

template <typename... Args>
void submit_counters(long long const timestamp_ns, char const *const name,
                     double const value, Args &&...args) noexcept {
  submit_counter(name, timestamp_ns, value);
  if constexpr (sizeof...(args) > 0) {
    submit_counters(timestamp_ns, std::forward<Args>(args)...);
  }
}

template <typename... Args>
void do_submit_counters(char const *const name, double const value,
                        Args &&...args) noexcept {
  auto const now_ns{impl::as_int_ns(impl::now())};
  submit_counters(now_ns, name, value, std::forward<Args>(args)...);
}

} // namespace cxxet::mark
