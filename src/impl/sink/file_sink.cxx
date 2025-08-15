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

#include "impl/sink/file_sink.hxx"

#include <iostream>

#include "impl/dump_records.hxx"

namespace cxxet::impl::sink {

template <bool thread_safe_v>
file_sink<thread_safe_v>::file_sink(long long const aTime_point_zero_ns,
                                    output::format const aFmt,
                                    char const *const aTarget_filename) noexcept
    : base_class_t{}, time_point_zero_ns{aTime_point_zero_ns}, fmt{aFmt},
      target_filename{aTarget_filename} {}

template <bool thread_safe_v>
file_sink<thread_safe_v>::file_sink(properties const &traits) noexcept
    : file_sink{traits.time_point_zero_ns, traits.default_target_format,
                traits.default_target_filename} {}

template <bool thread_safe_v> file_sink<thread_safe_v>::~file_sink() noexcept {
  do_flush();
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::flush(output::format const aFmt,
                                     char const *const aFilename) noexcept {
  std::lock_guard lck{*this};
  fmt = aFmt;
  target_filename = aFilename;
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::do_flush() noexcept {
  if (fmt == output::format::unknown) {
    std::cerr << "Forgot to specify output format (& filename)?!\n";
    return;
  }

  bool const target_filename_valid{
      (target_filename && target_filename[0] != '\0')};
  if (target_filename_valid) {
    if (!base_class_t::events.empty()) {
      try {
        std::array<char, L_tmpnam> tmp_filename_buffer{};
        auto const gen_tmp_filename = [&tmp_filename_buffer]() {
          // https://en.cppreference.com/w/cpp/io/c/tmpnam
          // "... std::tmpnam modifies static state and is not required to be
          // thread-safe ..." :'-(
          auto const ret{std::tmpnam(tmp_filename_buffer.data())};
          if (!ret) {
            throw std::runtime_error{"Failed to generate temporary filename"};
          }
          return ret;
        };
        // is `time_point_zero_ns` needed?!
        dump_records(base_class_t::events, time_point_zero_ns, fmt,
                     target_filename_valid ? target_filename
                                           : gen_tmp_filename());
        base_class_t::events.destroy();
      } catch (std::exception const &e) {
        std::cerr << "Failed to dump records: " << e.what() << '\n';
      }
    }
    // to avoid flushing again & rewriting the file implicitly ...:
    target_filename = nullptr;
  }
}

template struct file_sink<true>;
template struct file_sink<false>;

file_sink<true> &file_sink_global_instance() noexcept {
  static file_sink<true> instance{impl::sink::properties::instance()};
  return instance;
}

} // namespace cxxet::impl::sink
