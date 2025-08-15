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

#include "cxxet/all.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/sink_diversion.hxx"
#endif

namespace {

void flush_to_file_now([[maybe_unused]] char const *const filename,
                       [[maybe_unused]] bool const rereserve) {
#ifdef CXXET_ENABLE
  auto file_sink_local{cxxet::file_sink_handle::make(false)};
  file_sink_local->divert_thread_sink_to_this();
  file_sink_local->set_flush_target(cxxet::output::format::chrome_trace,
                                    filename);
#endif
  CXXET_sink_thread_flush_now();
  if (rereserve) {
    CXXET_sink_thread_reserve(1);
  }
#ifdef CXXET_ENABLE
  file_sink_local
      .reset(); // it would happen implicitly at the end of this scope, but ...
  CXXET_sink_thread_divert_to_sink_global(); // it's unsafe not to call this
                                             // after destroying (& flushing)
                                             // the `file_local_sink`.
#endif
}

} // namespace

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_thread_reserve();

  // NOTE this is very risky - by default, thread_local sink(s) flush to the
  // global one if they "overflow" (which doesn't happen in this artificial
  // example), but in general they may, and flushing them same way as here would
  // "split" the events into >= 2 files.
  {
    CXXET_mark_complete("complete");
  }
  flush_to_file_now(argc > 1 ? argv[1] : "/dev/stdout", true);

  CXXET_mark_instant("instant");
  flush_to_file_now(argc > 2 ? argv[2] : "/dev/stdout", true);

  CXXET_mark_counters("counter", 42.0);
  flush_to_file_now(argc > 3 ? argv[3] : "/dev/stdout", false);

  return 0;
}
