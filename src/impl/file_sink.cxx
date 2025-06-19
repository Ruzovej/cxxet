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

#include "impl/file_sink.hxx"

#include <iostream>

#include "impl/dump_records.hxx"

namespace cxxet::impl {

file_sink::file_sink(long long const aTime_point, output::format const aFmt,
                     char const *const aTarget_filename) noexcept
    : mutexed_sink{}, time_point{aTime_point}, fmt{aFmt},
      target_filename{aTarget_filename} {}

file_sink::file_sink(sink_properties const &traits) noexcept
    : file_sink{traits.time_point_zero_ns, traits.default_target_format,
                traits.default_target_filename} {}

file_sink::~file_sink() noexcept {
  std::lock_guard lck{get_mutex()};
  do_flush();
}

void file_sink::flush(output::format const aFmt, char const *const aFilename,
                      bool const defer) noexcept {
  std::lock_guard lck{get_mutex()};
  fmt = aFmt;
  target_filename = aFilename;
  if (!defer) {
    do_flush();
  }
}

void file_sink::do_flush() noexcept {
  if (!events.empty()) {
    try {
      if (target_filename) {
        // is `time_point_zero` needed?!
        dump_records(events, time_point, fmt, target_filename);
      }
      events.destroy();
    } catch (std::exception const &e) {
      std::cerr << "Failed to dump records: " << e.what() << '\n';
    }
  }
  // to avoid flushing again & rewriting the file implicitly ...:
  target_filename = nullptr;
}

} // namespace cxxet::impl
