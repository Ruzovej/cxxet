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

#include "shared_lib_foo.hxx"

#include <chrono>
#include <stdexcept>
#include <thread>

#include <cxxet/all.hxx>

namespace {
void pyramid(unsigned const level) {
  CXXET_mark_complete("foo_pyramid");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (level > 0) {
    pyramid(level - 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}
} // namespace

namespace shared_lib_foo {

void init() noexcept { CXXET_sink_thread_reserve(); }

void flush([[maybe_unused]] char const *const filename) noexcept {
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);
}

void perform_work(unsigned const i) noexcept { pyramid(i); }

some_class::some_class(int const ii) {
  // only even & positive args are allowed, throws `std::exception` derived
  // thing otherwise:
  if (ii < 0 || ii % 2 != 0) {
    throw std::runtime_error("Invalid argument: must be even and positive");
  }
  i = static_cast<unsigned>(ii);
}

void some_class::work() noexcept { perform_work(i); }

some_class make_some_class(int const i) { return some_class{i}; }

} // namespace shared_lib_foo
