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

  virtual void flush() noexcept = 0;

protected:
  virtual void *get_handle() noexcept = 0;

private:
  sink_handle(sink_handle const &) = delete;
  sink_handle &operator=(sink_handle const &) = delete;
  sink_handle(sink_handle &&) = delete;
  sink_handle &operator=(sink_handle &&) = delete;
};

struct CXXET_IMPL_API file_sink_handle : sink_handle {
  static std::unique_ptr<file_sink_handle>
  make(bool const thread_safe) noexcept;
  static std::unique_ptr<file_sink_handle>
  make(bool const thread_safe, output::format const aFmt,
       char const *const aTarget_filename) noexcept;

  virtual ~file_sink_handle() noexcept;

  virtual void flush_to(output::format const aFmt, char const *const aFilename,
                        bool const defer = false) noexcept = 0;
};

struct CXXET_IMPL_API cascade_sink_handle : sink_handle {
  static std::unique_ptr<cascade_sink_handle>
  make(bool const thread_safe,
       std::unique_ptr<sink_handle> const &aParent) noexcept;

  // after destroying/... this sink, don't forget to divert `thread_sink` to
  // another one, or to the default global file_sink (via
  // `sink_thread_divert_to_sink_global()`):
  virtual ~cascade_sink_handle() noexcept;

  virtual void divert_thread_sink_to_this() noexcept = 0;
};

} // namespace cxxet
