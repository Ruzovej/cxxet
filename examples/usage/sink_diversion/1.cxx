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

#include "cxxet/all.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/sink_diversion.hxx"
#endif

int main(int argc, char const **argv) {
#ifdef CXXET_ENABLE
  auto file_sink_local{cxxet::file_sink_handle::make(false)};
  file_sink_local->divert_thread_sink_to_this();
  [[maybe_unused]] char const *const filename1{argc > 1 ? argv[1]
                                                        : "/dev/stdout"};

  file_sink_local->flush(cxxet::output::format::chrome_trace, filename1);
#endif

  [[maybe_unused]] char const *const filename2{argc > 2 ? argv[2]
                                                        : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename2);

  CXXET_sink_thread_reserve();

  {
    CXXET_mark_complete(
        "example: redirecting all events to custom file_sink ... first");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_instant("within two sleeps ... first");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_sink_thread_flush(); // must be done before each sink switch (when not
                             // empty ...)

  CXXET_sink_thread_divert_to_sink_global();

  {
    CXXET_mark_complete("example: redirecting all events to default & global "
                        "file_sink ... first");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_instant("within two sleeps ... first");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_sink_thread_flush();

#ifdef CXXET_ENABLE
  file_sink_local->divert_thread_sink_to_this();
#endif

  {
    CXXET_mark_complete(
        "example: redirecting all events to custom file_sink ... second");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_instant("within two sleeps ... second");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_sink_thread_flush();

  CXXET_sink_thread_divert_to_sink_global();

  {
    CXXET_mark_complete("example: redirecting all events to default & global "
                        "file_sink ... second");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_instant("within two sleeps ... second");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_sink_thread_flush(); // when the target is the global one, it can be
                             // omitted. If it was some "local" one, it must be
                             // done before end of its lifetime

  return 0;
}
