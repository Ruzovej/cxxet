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

#include "cxxet/sink_control.hxx"

#include "impl/sink/event_collector.hxx"
#include "impl/sink/file_sink.hxx"

namespace cxxet {

#define CXXET_IMPL_GLOBAL_SINK impl::sink::file_sink_global_instance()
#define CXXET_IMPL_LOCAL_SINK                                                  \
  impl::sink::event_collector::thread_local_instance()

void sink_thread_reserve(int const minimum_free_capacity) noexcept {
  CXXET_IMPL_LOCAL_SINK.reserve(minimum_free_capacity);
}

void sink_thread_flush_now() noexcept { CXXET_IMPL_LOCAL_SINK.flush(); }

void sink_global_set_flush_target(std::string filename) noexcept {
  CXXET_IMPL_GLOBAL_SINK.set_flush_target(std::move(filename));
}

void sink_global_set_flush_target(
    std::unique_ptr<output::writer> custom_writer) noexcept {
  CXXET_IMPL_GLOBAL_SINK.set_flush_target(std::move(custom_writer));
}

void sink_thread_divert_to_sink_global() noexcept {
  CXXET_IMPL_LOCAL_SINK.set_parent(&CXXET_IMPL_GLOBAL_SINK);
}

#undef CXXET_IMPL_LOCAL_SINK
#undef CXXET_IMPL_GLOBAL_SINK

} // namespace cxxet
