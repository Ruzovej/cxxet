/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "cxxet/macros/linkage.h"
#include "cxxet/output_format.hxx"

namespace cxxet {

// call before submitting first marker; non-positive value - use
// env. or default setting
CXXET_IMPL_API void
sink_thread_reserve(int const minimum_free_capacity = 0) noexcept;

CXXET_IMPL_API void sink_thread_flush() noexcept;

CXXET_IMPL_API void sink_global_flush(
    cxxet::output::format const fmt = cxxet::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

} // namespace cxxet
