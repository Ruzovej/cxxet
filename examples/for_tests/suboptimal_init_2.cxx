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

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout", true);

  // suboptimal performance - will perform the initialization (reading env.
  // variables for the settings and buffer allocation) as part of the "event
  // flush". To prevent this, `CXXET_sink_thread_reserve(...)` should have been
  // called before first such marker:
  CXXET_mark_duration_begin("Suboptimal duration begin");
  // if more events, than for how many the thread has reserved capacity, are
  // submitted, the buffer will be (again suboptimally) allocated to the same
  // capacity as the previous call (in this case the default, implicit one).
  // User can manually prevent this by calling proper
  // `CXXET_sink_thread_reserve(...)`:
  //  1. with large enough value once, in the beginning
  //  2. in the "middle" of the work (but it will incur the allocation overhead
  //  there)
  do {
    CXXET_mark_complete("Some complete");
  } while (false); // ensures more readable formatting
  CXXET_mark_duration_end("Some duration end");

  return 0;
}
