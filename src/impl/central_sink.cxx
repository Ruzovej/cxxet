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

#include "impl/central_sink.hxx"

#include <iostream>

#include "impl/dump_records.hxx"

namespace cxxet::impl {

central_sink::central_sink(sink_properties const &traits)
    : time_point{traits.time_point_zero_ns}, fmt(traits.default_target_format),
      target_filename(traits.default_target_filename) {}

central_sink::~central_sink() noexcept {
  std::lock_guard lck{mtx};
  do_flush();
}

void central_sink::flush(output::format const aFmt, char const *const aFilename,
                         bool const defer) noexcept {
  std::lock_guard lck{mtx};
  fmt = aFmt;
  target_filename = aFilename;
  if (!defer) {
    do_flush();
  }
}

void central_sink::drain(sink &other) noexcept {
  std::lock_guard lck{mtx};
  sink::drain(other);
}

void central_sink::do_flush() noexcept {
  flush_to_file(time_point, fmt, target_filename);
  // to avoid flushing again & rewriting the file implicitly ...:
  target_filename = nullptr;
}

} // namespace cxxet::impl
