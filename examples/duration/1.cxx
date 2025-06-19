/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  CXXET_sink_thread_reserve();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    CXXET_mark_duration("RAII duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_mark_duration_begin("manual duration test");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_end();

  CXXET_mark_duration_begin("main - spawning threads");
  std::thread t1{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_duration("RAII thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
  std::thread t2{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_duration_begin("manual thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    CXXET_mark_duration_end();
  }};
  CXXET_mark_duration_end();

  // Test overlapping durations:
  {
    CXXET_mark_duration("RAII outer duration");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    {
      CXXET_mark_duration("RAII inner duration");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_mark_duration_begin("manual outer duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_begin("manual inner duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_end();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_end();

  constexpr int pyramid_height{6};
  for (int i{0}; i < pyramid_height; ++i) {
    CXXET_mark_duration_begin("Pyramid level");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for (int i{0}; i < pyramid_height; ++i) {
    CXXET_mark_duration_end();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_mark_duration_begin("main - joining threads");
  t1.join();
  t2.join();
  CXXET_mark_duration_end();

  return 0;
}
