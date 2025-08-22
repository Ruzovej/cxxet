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

#include <memory>
#include <string>

#include "cxxet/macros/linkage.h"
#include "cxxet/output/writer.hxx"

namespace cxxet {

// call before submitting first marker; non-positive value - use
// env. or default setting
CXXET_IMPL_API void
sink_thread_reserve(int const minimum_free_capacity = 0) noexcept;

CXXET_IMPL_API void sink_thread_flush_now() noexcept;

CXXET_IMPL_API void sink_global_set_flush_target(
    std::string filename =
        "" // empty => no-op; to be more precise: discard everything
    ) noexcept;

CXXET_IMPL_API void sink_global_set_flush_target(
    std::unique_ptr<output::writer> custom_writer) noexcept;

void sink_global_set_flush_target(std::nullptr_t) noexcept = delete;

CXXET_IMPL_API void sink_thread_divert_to_sink_global() noexcept;

} // namespace cxxet
