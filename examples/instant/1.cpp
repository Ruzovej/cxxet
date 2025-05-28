#include <iostream>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_instant.hpp"

int main(int argc, char const **argv) {
  RSM_init_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  // Unfortunately, using non-default scope (== `...::thread`), makes `chrome` &
  // ui.perfetto.dev display it somehow unusably ...

  RSM_MARK_INSTANT("macro: main thread started");

  std::thread{[]() { RSM_MARK_INSTANT("macro: thread 1 started"); }}.join();

  std::thread{[]() { RSM_MARK_INSTANT("macro: thread 2 started"); }}.join();

  std::thread{[]() { RSM_MARK_INSTANT("macro: thread 3 started"); }}.join();

  RSM_MARK_INSTANT("macro: main thread flushing all markers");

  RSM_flush_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_all_collected_events(rsm::output::format::chrome_trace, filename);

  return 0;
}
