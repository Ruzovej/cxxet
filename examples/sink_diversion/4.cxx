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
#include <string>
#include <thread>

#include "cxxet/all.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/sink_diversion.hxx"
#endif

static void test_block() {
  CXXET_sink_thread_reserve();

  CXXET_mark_complete("example: redirecting all events to custom file_sink");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_instant("within two sleeps");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename1{argc > 1 ? argv[1]
                                                        : "/dev/stdout"};

#ifdef CXXET_ENABLE
  auto file_sink_local{cxxet::file_sink_handle::make(true)};
  file_sink_local->flush(cxxet::output::format::chrome_trace, filename1, true);
#endif

  std::thread t1{[&]() {
#ifdef CXXET_ENABLE
    auto cascade_sink{cxxet::cascade_sink_handle::make(true, *file_sink_local)};
    cascade_sink->divert_thread_sink_to_this();
#endif

    std::thread t11{[&]() {
#ifdef CXXET_ENABLE
      cascade_sink->divert_thread_sink_to_this();
#endif
      test_block();
    }};

    std::thread t12{[&]() {
#ifdef CXXET_ENABLE
      cascade_sink->divert_thread_sink_to_this();
#endif
      test_block();
    }};

    test_block();

    t11.join();
    t12.join();

    CXXET_sink_thread_flush();
  }};

  std::thread t2{[&]() {
#ifdef CXXET_ENABLE
    auto cascade_sink{
        cxxet::cascade_sink_handle::make(false, *file_sink_local)};
    cascade_sink->divert_thread_sink_to_this();
#endif

    std::thread{[&]() {
#ifdef CXXET_ENABLE
      cascade_sink->divert_thread_sink_to_this();
#endif
      test_block();
    }}.join();

    test_block();

    std::thread t12{[&]() {
#ifdef CXXET_ENABLE
      cascade_sink->divert_thread_sink_to_this();
#endif
      test_block();
    }};

    t12.join();

    CXXET_sink_thread_flush();
  }};

#ifdef CXXET_ENABLE
  auto cascade_sink{cxxet::cascade_sink_handle::make(false, *file_sink_local)};
  cascade_sink->divert_thread_sink_to_this();
#endif

  test_block();

  CXXET_sink_thread_flush();

  t1.join();
  t2.join();

#ifdef CXXET_ENABLE
  file_sink_local->divert_thread_sink_to_this();
#endif

  test_block();

  CXXET_sink_thread_flush();

  return 0;
}
