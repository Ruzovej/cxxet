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

#include <mutex>

#include "cxxet/output_format.hxx"
#include "impl/sink.hxx"
#include "impl/sink_properties.hxx"

namespace cxxet::impl {

struct central_sink : sink {
  explicit central_sink(sink_properties const &traits);
  ~central_sink() noexcept override;

  void flush(output::format const aFmt, char const *const aFilename,
             bool const defer) noexcept;

  void drain(sink &other) noexcept override final;

private:
  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  void do_flush() noexcept;

  std::mutex mtx;
  long long const time_point;
  output::format fmt;
  char const *target_filename;
};

} // namespace cxxet::impl
