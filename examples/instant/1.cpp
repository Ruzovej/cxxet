#include <chrono>
#include <thread>

#include "cxxst/all.hpp"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXST_sink_global_flush(cxxst::output::format::chrome_trace, filename, true);

  CXXST_thread_local_sink_reserve();

  CXXST_mark_instant("main thread started");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // Unfortunately, using non-default scope (== `...::thread`), makes `chrome` &
  // ui.perfetto.dev display it somehow unusably ...

  std::thread t1{[]() {
    CXXST_thread_local_sink_reserve();
    CXXST_mark_instant("thread 1 started");
  }};

  std::thread t2{[]() {
    CXXST_thread_local_sink_reserve();
    CXXST_mark_instant("thread 2 started");
  }};

  std::thread t3{[]() {
    CXXST_thread_local_sink_reserve();
    CXXST_mark_instant("thread 3 started");
  }};

  CXXST_mark_instant("main thread flushing all markers");

  CXXST_sink_thread_flush();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  t1.join();
  t2.join();
  t3.join();

  return 0;
}
