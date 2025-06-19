/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include <climits>
#include <cstdlib>

#include <new>
#include <string_view>
#include <thread>

namespace {

void trigger_tsan() {
  volatile int i{0};
  std::thread th{[&i]() {
    for (int j{0}; j < 1'000'000; ++j) {
      ++i;
    }
  }};
  for (int j{0}; j < 1'000'000; ++j) {
    ++i;
  }
  th.join();
}

void trigger_ubsan() {
  int i{-1};
  i <<= 1;
}

void trigger_asan() {
  [[maybe_unused]] int a[2], b[2], c[2];
  volatile unsigned index{2};
  b[index] = 1;
}

void trigger_lsan() {
  [[maybe_unused]] auto *volatile p{new int{}};
  [[maybe_unused]] auto *volatile up{std::make_unique<int>().release()};
}

} // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  const std::string_view arg{argv[1]};
  if (arg == "tsan") {
    trigger_tsan();
  } else if (arg == "ubsan") {
    trigger_ubsan();
  } else if (arg == "asan") {
    trigger_asan();
  } else if (arg == "lsan") {
    trigger_lsan();
  } else {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
