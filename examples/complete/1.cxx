/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

static void pyramid(int const level) {
  CXXET_mark_complete(__FUNCTION__);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (level > 0) {
    pyramid(level - 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main(int argc, char const **argv) {
  CXXET_sink_thread_reserve();
  CXXET_mark_complete(__FUNCTION__);

  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  std::thread t1{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_complete("scope 1.1");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      CXXET_mark_complete("scope 2.1");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      CXXET_mark_complete("scope 2.2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};

  std::thread t2{[]() {
    CXXET_sink_thread_reserve();
    pyramid(4);
  }};

  pyramid(3);

  t1.join();
  t2.join();

  return 0;
}
