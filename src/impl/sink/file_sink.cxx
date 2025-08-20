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

#include "impl/default_writer.hxx"
#include "impl/dump_records.hxx"
#include "impl/tmp_filename_handle.hxx"

namespace cxxet::impl::sink {

template <bool thread_safe_v>
file_sink<thread_safe_v>::file_sink(long long const aTime_point_zero_ns,
                                    output::format const aFmt,
                                    std::string &&aTarget_filename) noexcept
    : base_class_t{}, time_point_zero_ns{aTime_point_zero_ns}, fmt{aFmt},
      target_filename{std::move(aTarget_filename)} {}

template <bool thread_safe_v>
file_sink<thread_safe_v>::file_sink(properties const &traits) noexcept
    : file_sink{traits.time_point_zero_ns, traits.default_target_format,
                traits.default_target_filename} {}

template <bool thread_safe_v> file_sink<thread_safe_v>::~file_sink() noexcept {
  do_flush();
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::set_flush_target(
    output::format const aFmt, std::string &&aFilename) noexcept {
  std::lock_guard lck{*this};
  fmt = aFmt;
  target_filename = std::move(aFilename);
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::set_flush_target(
    output::format const aFmt,
    std::unique_ptr<output::writer> &&aCustom_writer) noexcept {
  std::lock_guard lck{*this};
  fmt = aFmt;
  custom_writer = std::move(aCustom_writer);
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::do_flush() noexcept {
  if (fmt == output::format::unknown) {
    std::cerr << "Forgot to specify output format?!\n";
    return;
  }

  if (!base_class_t::events.empty()) {
    try {
      if (custom_writer) {
        dump_records(base_class_t::events, time_point_zero_ns, fmt,
                     *custom_writer);
      } else if (!target_filename.empty()) {
        tmp_filename_handle implicit_file_handle{target_filename};
        bool const use_tmp_filename{
            tmp_filename_handle::valid_base(target_filename)};
        auto const target{use_tmp_filename
                              ? static_cast<char const *>(implicit_file_handle)
                              : target_filename.c_str()};
        if (use_tmp_filename) {
          std::cerr << "Saving events to file: "
                    << static_cast<std::string_view>(target) << '\n';
        }

        // is `time_point_zero_ns` needed?!
        default_writer def_writer{target};
        dump_records(base_class_t::events, time_point_zero_ns, fmt, def_writer);
      }
    } catch (std::exception const &e) {
      std::cerr << "Failed to dump records: " << e.what() << '\n';
    }
    base_class_t::events.destroy();
  }
}

template struct file_sink<true>;
template struct file_sink<false>;

file_sink<true> &file_sink_global_instance() noexcept {
  static file_sink<true> instance{impl::sink::properties::instance()};
  return instance;
}

} // namespace cxxet::impl::sink
