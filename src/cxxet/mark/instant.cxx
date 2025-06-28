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

#include "cxxet/mark/instant.hxx"

#include "impl/event/kind/instant.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxet::mark {

void submit_instant(char const *const desc, scope_t const scope,
                    long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::instant{desc, scope, timestamp_ns});
}

} // namespace cxxet::mark
