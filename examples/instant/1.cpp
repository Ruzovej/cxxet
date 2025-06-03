#include <chrono>
#include <thread>

#include "rsm/all.hpp"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXST_flush_global_sink(rsm::output::format::chrome_trace, filename, true);

  CXXST_init_thread_local_sink();

  CXXST_mark_instant("main thread started");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // Unfortunately, using non-default scope (== `...::thread`), makes `chrome` &
  // ui.perfetto.dev display it somehow unusably ...

  std::thread t1{[]() { CXXST_mark_instant("thread 1 started"); }};

  std::thread t2{[]() { CXXST_mark_instant("thread 2 started"); }};

  std::thread t3{[]() { CXXST_mark_instant("thread 3 started"); }};

  CXXST_mark_instant("main thread flushing all markers");

  CXXST_flush_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  t1.join();
  t2.join();
  t3.join();

  return 0;
}
