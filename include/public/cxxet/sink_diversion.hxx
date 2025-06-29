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

#include "cxxet/macros/linkage.h"
#include "cxxet/output_format.hxx"

namespace cxxet {

struct CXXET_IMPL_API sink_handle {
  virtual ~sink_handle() noexcept;

  virtual void divert_thread_sink_to_this() noexcept = 0;
};

struct CXXET_IMPL_API file_sink_handle : sink_handle {
  static std::unique_ptr<file_sink_handle>
  make(bool const thread_safe) noexcept;

  virtual void flush(output::format const fmt, char const *const filename,
                     bool const defer) noexcept = 0;
};

struct CXXET_IMPL_API cascade_sink_handle : sink_handle {
  static std::unique_ptr<cascade_sink_handle>
  make(bool const thread_safe, sink_handle &parent) noexcept;

  virtual void flush() noexcept = 0;
};

} // namespace cxxet
