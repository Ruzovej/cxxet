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
#ifdef CXXET_ENABLE
#include "cxxet/sink_diversion.hxx"
#endif

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename1{argc > 1 ? argv[1]
                                                        : "/dev/stdout"};
  [[maybe_unused]] char const *const filename2{argc > 2 ? argv[2]
                                                        : "/dev/stdout"};

#ifdef CXXET_ENABLE
  auto file_sink_local1{cxxet::file_sink_handle::make(false)};
  file_sink_local1->set_flush_target(filename1);

  auto file_sink_local2{cxxet::file_sink_handle::make(false)};
  file_sink_local2->set_flush_target(filename2);
#endif

  std::thread t1{[&]() {
#ifdef CXXET_ENABLE
    file_sink_local1->divert_thread_sink_to_this();
#endif
    CXXET_sink_thread_reserve();

    CXXET_mark_complete("example: redirecting all events to custom file_sink 1 "
                        "(spawned thread)");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_instant("within two sleeps 1 (spawned thread)");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};

  std::thread t2{[&]() {
#ifdef CXXET_ENABLE
    file_sink_local2->divert_thread_sink_to_this();
#endif
    CXXET_sink_thread_reserve();

    CXXET_mark_complete("example: redirecting all events to custom file_sink 2 "
                        "(spawned thread)");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_instant("within two sleeps 2 (spawned thread)");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};

  t1.join();
  t2.join();

  {
#ifdef CXXET_ENABLE
    file_sink_local1->divert_thread_sink_to_this();
#endif
    CXXET_sink_thread_reserve();

    {
      CXXET_mark_complete(
          "example: redirecting all events to custom file_sink 1 "
          "(main thread)");

      std::this_thread::sleep_for(std::chrono::milliseconds(1));

      CXXET_mark_instant("within two sleeps 1 (main thread)");

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CXXET_sink_thread_flush_now();
  }

  {
#ifdef CXXET_ENABLE
    file_sink_local2->divert_thread_sink_to_this();
#endif
    CXXET_sink_thread_reserve();

    {
      CXXET_mark_complete(
          "example: redirecting all events to custom file_sink 2 "
          "(main thread)");

      std::this_thread::sleep_for(std::chrono::milliseconds(1));

      CXXET_mark_instant("within two sleeps 2 (main thread)");

      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    CXXET_sink_thread_flush_now();
  }

  return 0;
}
