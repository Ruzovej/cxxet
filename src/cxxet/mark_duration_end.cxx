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

#include "cxxet/mark_duration_end.hxx"

#include "impl/event/kind/duration.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxet {

void submit_duration_end(char const *const desc,
                         long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::duration_end{desc, timestamp_ns});
}

} // namespace cxxet
