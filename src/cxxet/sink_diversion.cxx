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

#include "cxxet/sink_diversion.hxx"

#include "cxxet/output_format.hxx"
#include "impl/sink/cascade.hxx"
#include "impl/sink/event_collector.hxx"
#include "impl/sink/file_sink.hxx"

namespace cxxet {

sink_handle::sink_handle() noexcept = default;

sink_handle::~sink_handle() noexcept = default;

namespace {
template <bool thread_safe_v>
struct file_sink_handle_impl final : file_sink_handle {
  file_sink_handle_impl(output::format const fmt = output::format::chrome_trace,
                        char const *const filename = nullptr) noexcept
      : file_sink_handle{},
        sink{impl::sink::properties::instance().time_point_zero_ns, fmt,
             filename} {}

  ~file_sink_handle_impl() noexcept override = default;

  void divert_thread_sink_to_this() noexcept override {
    impl::sink::event_collector::thread_local_instance().set_parent(&sink);
  }

  void *get_handle() noexcept override { return &sink; }

  void flush(output::format const fmt, char const *const filename,
             bool const defer) noexcept override {
    sink.flush(fmt, filename, defer);
  }

private:
  impl::sink::file_sink<thread_safe_v> sink;
};
} // namespace

std::unique_ptr<file_sink_handle>
file_sink_handle::make(bool const thread_safe) noexcept {
  if (thread_safe) {
    return std::make_unique<file_sink_handle_impl<true>>();
  } else {
    return std::make_unique<file_sink_handle_impl<false>>();
  }
}

file_sink_handle::~file_sink_handle() noexcept = default;

namespace {
template <bool thread_safe_v>
struct cascade_sink_handle_impl final : cascade_sink_handle {
  cascade_sink_handle_impl(sink_handle *const parent) noexcept
      : sink{reinterpret_cast<impl::sink::sink_base *>(parent->get_handle())} {}

  ~cascade_sink_handle_impl() noexcept override = default;

  void divert_thread_sink_to_this() noexcept override {
    impl::sink::event_collector::thread_local_instance().set_parent(&sink);
  }

  void *get_handle() noexcept override { return &sink; }

  void flush() noexcept override { sink.flush(); }

private:
  impl::sink::cascade<thread_safe_v> sink;
};
} // namespace

std::unique_ptr<cascade_sink_handle>
cascade_sink_handle::make(bool const thread_safe,
                          std::unique_ptr<sink_handle> const &parent) noexcept {
  if (thread_safe) {
    return std::make_unique<cascade_sink_handle_impl<true>>(parent.get());
  } else {
    return std::make_unique<cascade_sink_handle_impl<false>>(parent.get());
  }
}

cascade_sink_handle::~cascade_sink_handle() noexcept = default;

} // namespace cxxet
