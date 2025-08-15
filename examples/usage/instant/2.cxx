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

#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_set_flush_target(cxxet::output::format::chrome_trace,
                                     filename);

  CXXET_sink_thread_reserve();

  CXXET_mark_instant("main thread beginning");

  {
    CXXET_mark_complete("main thread, local scope");

    // Unfortunately, using non-default scope (== `cxxet::scope_t::thread`),
    // makes `chrome` & ui.perfetto.dev display it somehow unusably ...

    std::thread t1{[]() {
      CXXET_sink_thread_reserve();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      CXXET_mark_complete("thread 1");
      CXXET_mark_instant("thread 1 started");
    }};

    std::thread t2{[]() {
      CXXET_sink_thread_reserve();
      CXXET_mark_complete("thread 2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      CXXET_mark_instant("thread 2 started");
    }};

    std::thread t3{[]() {
      CXXET_sink_thread_reserve();
      CXXET_mark_complete("thread 3");
      CXXET_mark_instant("thread 3 started");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }};

    t1.join();
    t2.join();
    t3.join();
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_instant("main terminating");

  return 0;
}
