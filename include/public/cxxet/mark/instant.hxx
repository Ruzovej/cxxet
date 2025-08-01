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
#include "cxxet/scope.hxx"
#include "cxxet/timepoint.hxx"

namespace cxxet::mark {

CXXET_IMPL_API void submit_instant(char const *const desc, scope_t const scope,
                                   long long const timestamp_ns) noexcept;

inline void instant(char const *const desc,
                    scope_t const scope = scope_t::thread) noexcept {
  submit_instant(desc, scope, impl::as_int_ns(impl::now()));
}

} // namespace cxxet::mark
