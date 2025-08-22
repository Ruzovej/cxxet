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

#include "cxxet/output/writer.hxx"
#include "impl/sink/properties.hxx"
#include "impl/sink/thread_safe_t.hxx"

namespace cxxet::impl::sink {

template <bool thread_safe_v> struct file_sink : thread_safe_t<thread_safe_v> {
  using base_class_t = thread_safe_t<thread_safe_v>;

  explicit file_sink(long long const aTime_point_zero_ns,
                     std::string &&aTarget_filename = "") noexcept;
  explicit file_sink(properties const &traits) noexcept;
  ~file_sink() noexcept override;

  void set_flush_target(std::string &&aFilename) noexcept;
  void
  set_flush_target(std::unique_ptr<output::writer> &&aCustom_writer) noexcept;

private:
  file_sink(file_sink const &) = delete;
  file_sink &operator=(file_sink const &) = delete;
  file_sink(file_sink &&) = delete;
  file_sink &operator=(file_sink &&) = delete;

  void write_out_events() noexcept;

  // TODO (https://github.com/Ruzovej/cxxet/issues/133) is `time_point_zero_ns`
  // needed?!
  long long const time_point_zero_ns;
  // TODO (https://github.com/Ruzovej/cxxet/issues/132) std::variant (or
  // similar) of
  //  1. std::string (target_filename)
  //  2. std::unique_ptr<output::writer> (custom_writer)
  std::string target_filename;
  std::unique_ptr<output::writer> custom_writer{nullptr};
};

extern template struct file_sink<true>;
extern template struct file_sink<false>;

file_sink<true> &file_sink_global_instance() noexcept;

} // namespace cxxet::impl::sink
