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

#include <thread>

#include "cxxet/all.hxx"

namespace {

void record_some_events() {
  CXXET_sink_thread_reserve();

  CXXET_mark_complete("a complete event that disappears");

  CXXET_mark_instant("event that will never be seen");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counter("lost counter", 42.0);

  CXXET_sink_thread_flush_now();
}

} // namespace

int main(int const, char const **) {
  std::thread t{record_some_events};

  record_some_events();

  CXXET_sink_global_set_flush_target(
      cxxet::output::format::chrome_trace,
      "" // file won't be created/overwritten - empty string means to discard
         // all recorded events ...
  );

  t.join();

  return 0;
}
