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

#include "cxxet/basic.hxx"

int main(int argc, char const **argv) {
  CXXET_mark_complete(__FUNCTION__);

  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_set_flush_target(filename);

  CXXET_sink_thread_reserve();

  {
    CXXET_mark_complete("setting metadata in main thread");
    // meaningless to set more than once:
    CXXET_mark_process_name(argv[0]);
    CXXET_mark_process_sort_index(5);
    CXXET_mark_thread_name("main thread in this example :-)");
    CXXET_mark_thread_sort_index(10);
    // set as many as needed (those are probably ignored by `ui.perfetto.dev`):
    CXXET_mark_process_label("test_label_1");
    CXXET_mark_process_label("test_label_2");
    CXXET_mark_process_label("test label 3");
  }

  CXXET_mark_duration_begin("spawning thread");
  std::thread t{[]() {
    CXXET_sink_thread_reserve();

    {
      CXXET_mark_complete("setting metadata in separate thread");
      CXXET_mark_thread_name("spawned thread");
      CXXET_mark_thread_sort_index(50);
    }

    CXXET_mark_duration("traced event");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
  CXXET_mark_duration_end();

  CXXET_mark_instant("main - joining thread");
  t.join();
  CXXET_mark_instant("main - thread joined");

  return 0;
}
