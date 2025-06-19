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

static void thread_local_sink_lifecycle() {
  CXXET_sink_thread_reserve(1);
  // do whatever You need between those ...
  CXXET_sink_thread_flush(); // not necessary, it will be flushed
                             // implicitly too - unless You want to do
                             // more work after that ...

  // more work, that doesn't submit any `cxxet` events
}

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  std::thread{thread_local_sink_lifecycle}.join();
  std::thread t1{thread_local_sink_lifecycle};
  std::thread t2{thread_local_sink_lifecycle};

  thread_local_sink_lifecycle();

  t1.join();
  std::thread{thread_local_sink_lifecycle}.join();
  t2.join();

  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout");

  return 0;
}
