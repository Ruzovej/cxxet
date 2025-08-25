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
#include "impl/tmp_filename_handle.hxx"
#include "impl/write_out/in_trace_event_format.hxx"

namespace cxxet::impl::sink {

template <bool thread_safe_v>
file_sink<thread_safe_v>::file_sink(long long const aTime_point_zero_ns,
                                    std::string &&aTarget_filename) noexcept
    : base_class_t{}, time_point_zero_ns{aTime_point_zero_ns},
      target_filename{std::move(aTarget_filename)} {}

template <bool thread_safe_v>
file_sink<thread_safe_v>::file_sink(properties const &traits) noexcept
    : file_sink{traits.time_point_zero_ns, traits.default_target_filename} {}

template <bool thread_safe_v> file_sink<thread_safe_v>::~file_sink() noexcept {
  write_out_events();
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::set_flush_target(
    std::string &&aFilename) noexcept {
  std::lock_guard lck{*this};
  target_filename = std::move(aFilename);
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::set_flush_target(
    std::unique_ptr<output::writer> &&aCustom_writer) noexcept {
  std::lock_guard lck{*this};
  custom_writer = std::move(aCustom_writer);
}

template <bool thread_safe_v>
unsigned file_sink<thread_safe_v>::register_category_name(
    unsigned const category, std::string &&name, bool const allow_rename) {
  std::lock_guard lck{*this};
  return category_names.register_category_name(category, std::move(name),
                                               allow_rename);
}

template <bool thread_safe_v>
void file_sink<thread_safe_v>::write_out_events() noexcept {
  if (!base_class_t::events.empty()) {
    try {
      if (custom_writer) {
        write_out::in_trace_event_format(*custom_writer, time_point_zero_ns,
                                         base_class_t::events);
      } else if (!target_filename.empty()) {
        tmp_filename_handle implicit_file_handle{target_filename};
        char const *target{};
        if (tmp_filename_handle::valid_base(target_filename)) {
          std::cerr << "Writing out events to file: "
                    << static_cast<std::string_view>(implicit_file_handle)
                    << '\n';
          target = static_cast<char const *>(implicit_file_handle);
        } else {
          target = target_filename.c_str();
        }
        default_writer def_writer{target};
        write_out::in_trace_event_format(def_writer, time_point_zero_ns,
                                         base_class_t::events);
      }
    } catch (std::exception const &e) {
      std::cerr << "Failed to write out events: " << e.what() << '\n';
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
