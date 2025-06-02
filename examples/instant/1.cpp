#include <chrono>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_instant.hpp"

int main(int argc, char const **argv) {
  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_global_sink(rsm::output::format::chrome_trace, filename, true);

  RSM_init_thread_local_sink();

  RSM_MARK_INSTANT("main thread started");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // Unfortunately, using non-default scope (== `...::thread`), makes `chrome` &
  // ui.perfetto.dev display it somehow unusably ...

  std::thread t1{[]() { RSM_MARK_INSTANT("thread 1 started"); }};

  std::thread t2{[]() { RSM_MARK_INSTANT("thread 2 started"); }};

  std::thread t3{[]() { RSM_MARK_INSTANT("thread 3 started"); }};

  RSM_MARK_INSTANT("main thread flushing all markers");

  RSM_flush_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  t1.join();
  t2.join();
  t3.join();

  return 0;
}
