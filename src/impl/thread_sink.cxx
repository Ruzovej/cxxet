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

#include "impl/thread_sink.hxx"

#include <cassert>

namespace cxxet::impl {

thread_sink::thread_sink(sink *aParent) noexcept : cascade_sink{aParent} {}

thread_sink::~thread_sink() noexcept = default;

void thread_sink::append_event(event::any const &evt) noexcept {
  events.safe_append(evt, default_node_capacity);
}

void thread_sink::reserve(int const minimum_free_capacity) noexcept {
  assert(minimum_free_capacity > 0);
  default_node_capacity = minimum_free_capacity;
  events.reserve(default_node_capacity);
}

} // namespace cxxet::impl
