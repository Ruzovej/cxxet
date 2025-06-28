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

#include "cxxet/output_format.hxx"
#include "impl/sink/properties.hxx"
#include "impl/sink/thread_safe_t.hxx"

namespace cxxet::impl::sink {

template <bool thread_safe_v> struct file_sink : thread_safe_t<thread_safe_v> {
  explicit file_sink(long long const aTime_point_zero_ns) noexcept;
  explicit file_sink(long long const aTime_point_zero_ns,
                     output::format const aFmt,
                     char const *const aTarget_filename) noexcept;
  explicit file_sink(properties const &traits) noexcept;
  ~file_sink() noexcept override;

  void flush(output::format const aFmt, char const *const aFilename,
             bool const defer) noexcept;

private:
  file_sink(file_sink const &) = delete;
  file_sink &operator=(file_sink const &) = delete;
  file_sink(file_sink &&) = delete;
  file_sink &operator=(file_sink &&) = delete;

  using base_class_t = thread_safe_t<thread_safe_v>;

  void do_flush() noexcept;

  long long const time_point_zero_ns;
  output::format fmt{output::format::unknown};
  char const *target_filename{nullptr};
};

extern template struct file_sink<true>;
extern template struct file_sink<false>;

file_sink<true> &file_sink_global_instance() noexcept;

} // namespace cxxet::impl::sink
