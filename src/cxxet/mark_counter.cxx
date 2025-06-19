/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include "cxxet/mark_counter.hxx"

#include "impl/event/kind/complete.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxet {

void submit_counter(char const *const name, long long const timestamp_ns,
                    double const value) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::counter{name, timestamp_ns, value});
}

} // namespace cxxet
