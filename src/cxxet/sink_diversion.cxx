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

#include <cassert>

#include "impl/sink/cascade.hxx"
#include "impl/sink/event_collector.hxx"
#include "impl/sink/file_sink.hxx"

namespace cxxet {

sink_handle::~sink_handle() noexcept = default;

namespace {

struct sink_handle_provider {
  virtual ~sink_handle_provider() noexcept = default;
  virtual impl::sink::sink_base *get_raw_sink_handle() noexcept = 0;
};

template <bool thread_safe_v>
struct file_sink_handle_impl final : file_sink_handle, sink_handle_provider {
  file_sink_handle_impl() noexcept
      : file_sink_handle{}, sink_handle_provider{},
        sink{impl::sink::properties::instance().time_point_zero_ns} {}

  ~file_sink_handle_impl() noexcept override = default;

  void divert_thread_sink_to_this() noexcept override {
    impl::sink::event_collector::thread_local_instance().set_parent(&sink);
  }

  impl::sink::sink_base *get_raw_sink_handle() noexcept override {
    return &sink;
  }

  void set_flush_target(std::string filename) noexcept override {
    sink.set_flush_target(std::move(filename));
  }

  void set_flush_target(
      std::unique_ptr<output::writer> custom_writer) noexcept override {
    assert(custom_writer);
    sink.set_flush_target(std::move(custom_writer));
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

namespace {
template <bool thread_safe_v>
struct cascade_sink_handle_impl final : cascade_sink_handle,
                                        sink_handle_provider {
  cascade_sink_handle_impl(sink_handle *parent) noexcept
      : cascade_sink_handle{}, sink_handle_provider{},
        sink{dynamic_cast<sink_handle_provider *>(parent)
                 ->get_raw_sink_handle()} {}

  ~cascade_sink_handle_impl() noexcept override = default;

  void divert_thread_sink_to_this() noexcept override {
    impl::sink::event_collector::thread_local_instance().set_parent(&sink);
  }

  impl::sink::sink_base *get_raw_sink_handle() noexcept override {
    return &sink;
  }

  void flush_now() noexcept override { sink.flush(); }

private:
  impl::sink::cascade<thread_safe_v> sink;
};
} // namespace

std::unique_ptr<cascade_sink_handle>
cascade_sink_handle::make(bool const thread_safe,
                          sink_handle &parent) noexcept {
  if (thread_safe) {
    return std::make_unique<cascade_sink_handle_impl<true>>(&parent);
  } else {
    return std::make_unique<cascade_sink_handle_impl<false>>(&parent);
  }
}

} // namespace cxxet
