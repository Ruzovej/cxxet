/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include <condition_variable>
#include <mutex>
#include <thread>

#include "cxxet/all.hxx"

namespace {

std::mutex go_mtx;
std::condition_variable go_cv;
bool go{false};

void record_some_events(bool const wait) {
  CXXET_sink_thread_reserve();

  CXXET_mark_complete("a complete event that disappears");

  CXXET_mark_instant("event that will never be seen");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counters("lost counter", 42.0);

  // doesn't explicitly flush the thread-local sink ... which is implicitly done
  // at thread termination.

  if (wait) {
    std::unique_lock<std::mutex> lock(go_mtx);
    go_cv.wait(lock, []() { return go; });
  }
}

} // namespace

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  std::thread t1{record_some_events, true};

  // Record events in main thread
  record_some_events(false);
  // but don't flush the thread-local sink...

  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout"); // will be empty

  {
    std::lock_guard<std::mutex> lock(go_mtx);
    go = true;
  }
  go_cv.notify_one();
  t1.join();

  // won't contain markers from main thread ... forgot to explicitly flush them
  // -> they will be implicitly flushed on thread exit ...:
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 2 ? argv[2] : "/dev/stdout");

  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 3 ? argv[3] : "/dev/stdout",
                          true); // deferred flush ... will happen after
  // implicitly flushing thread_local sink. In this particular case, it's, in
  // the end, equivalent to this (with explicit flush):
  /*
    CXXET_sink_thread_flush();
    CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                            argc > 3 ? argv[3] : "/dev/stdout");
  */
  return 0;
}
