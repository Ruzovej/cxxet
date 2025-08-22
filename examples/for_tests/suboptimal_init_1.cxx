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

#include "cxxet/basic.hxx"

static void improper_cxxet_usage() {
  // `CXXET_sink_thread_reserve(...)` should have been called, this would have
  // suboptimal performance in case any markers are used
  CXXET_sink_thread_flush_now();
}

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_global_set_flush_target(cxxet::output::format::chrome_trace,
                                     argc > 1 ? argv[1] : "/dev/stdout");

  std::thread t{improper_cxxet_usage};

  improper_cxxet_usage();

  t.join();

  return 0;
}
