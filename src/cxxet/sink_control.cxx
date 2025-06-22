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
#include "impl/thread_local_sink_submit_event.hxx"

#include <cassert>

#include <iostream> // must be there, even if unused ... so `std::cout`, etc. gets initialized before it's potentially used in `sink_properties` ctor
#include <optional>

#include "impl/file_sink.hxx"
#include "impl/sink_properties.hxx"
#include "impl/thread_sink.hxx"

namespace cxxet {

namespace {
impl::sink_properties const sink_props{};
impl::file_sink global_sink{sink_props};
thread_local std::optional<impl::thread_sink> local_sink;
} // namespace

void sink_thread_reserve(int const minimum_free_capacity) noexcept {
  if (local_sink == std::nullopt) {
    local_sink.emplace(&global_sink);
  }
  local_sink->reserve(minimum_free_capacity <= 0
                          ? sink_props.default_list_node_capacity
                          : minimum_free_capacity);
}

void sink_thread_flush() noexcept {
  assert(local_sink != std::nullopt && "thread local sink not initialized!");
  local_sink->flush();
}

void sink_global_flush(cxxet::output::format const fmt,
                       char const *const filename,
                       bool const defer_flush) noexcept {
  global_sink.flush(fmt, filename, defer_flush);
}

void sink_thread_divert_to_sink_global() noexcept {
  assert(local_sink != std::nullopt && "thread local sink not initialized!");
  local_sink->set_parent(&global_sink);
}

namespace impl {

void thread_local_sink_submit_event(event::any const &evt) noexcept {
  assert(local_sink != std::nullopt && "thread local sink not initialized!");
  local_sink->append_event(evt);
}

} // namespace impl
} // namespace cxxet
