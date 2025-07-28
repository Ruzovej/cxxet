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

#include "impl/sink/event_collector.hxx"

#include "impl/sink/file_sink.hxx"
#include "impl/sink/properties.hxx"

namespace cxxet::impl::sink {

void event_collector::append_event(event::any const &evt) noexcept {
  events.safe_append(evt, default_node_capacity);
}

void event_collector::reserve(int const minimum_free_capacity) noexcept {
  default_node_capacity =
      minimum_free_capacity <= 0
          ? impl::sink::properties::instance().default_list_node_capacity
          : minimum_free_capacity;

  events.reserve(default_node_capacity);
}

event_collector &event_collector::thread_local_instance() noexcept {
  thread_local impl::sink::event_collector local_sink{
      &sink::file_sink_global_instance() // default parent
  };
  return local_sink;
}

event_collector::event_collector(sink_base *aParent) noexcept
    : cascade{aParent},
      default_node_capacity{
          impl::sink::properties::instance().default_list_node_capacity} {}

event_collector::~event_collector() noexcept = default;

} // namespace cxxet::impl::sink
