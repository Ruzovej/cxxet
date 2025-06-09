#include <condition_variable>
#include <mutex>
#include <thread>

#include "cxxst/all.hpp"

namespace {

std::mutex go_mtx;
std::condition_variable go_cv;
bool go{false};

void record_some_events(bool const wait) {
  CXXST_thread_local_sink_reserve();

  CXXST_mark_complete("a complete event that disappears");

  CXXST_mark_instant("event that will never be seen");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXST_mark_counters("lost counter", 42.0);

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

  CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout"); // will be empty

  {
    std::lock_guard<std::mutex> lock(go_mtx);
    go = true;
  }
  go_cv.notify_one();
  t1.join();

  // won't contain markers from main thread ... forgot to explicitly flush them
  // -> they will be implicitly flushed on thread exit ...:
  CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                          argc > 2 ? argv[2] : "/dev/stdout");

  CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                          argc > 3 ? argv[3] : "/dev/stdout",
                          true); // deferred flush ... will happen after
  // implicitly flushing thread_local sink. In this particular case, it's, in
  // the end, equivalent to this (with explicit flush):
  /*
    CXXST_sink_thread_flush();
    CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                            argc > 3 ? argv[3] : "/dev/stdout");
  */
  return 0;
}
