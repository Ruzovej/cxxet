/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include "cxxet/all.hxx"

namespace {

void flush_to_file_now(char const *const filename, bool const rereserve) {
  CXXET_sink_thread_flush();
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename);
  if (rereserve) {
    CXXET_sink_thread_reserve(1);
  }
}

} // namespace

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_thread_reserve();

  // honestly, this functionality isn't prepared for multithreading ... since
  // all thread_local sinks flush to the same global one (protected by
  // locked`std::mutex` of course).

  { CXXET_mark_complete("complete"); }
  flush_to_file_now(argc > 1 ? argv[1] : "/dev/stdout", true);

  CXXET_mark_instant("instant");
  flush_to_file_now(argc > 2 ? argv[2] : "/dev/stdout", true);

  CXXET_mark_counters("counter", 42.0);
  flush_to_file_now(argc > 3 ? argv[3] : "/dev/stdout", false);

  return 0;
}
