#include <iostream>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_counter.hpp"

int main(int argc, char const **argv) {
  RSM_init_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  RSM_MARK_COUNTERS("RAM [MB]", 1.1, "cpu utilization", 42.0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  RSM_MARK_COUNTERS("RAM [MB]", 2.2, "cpu utilization", 22.0);

  std::thread{[]() {
    RSM_MARK_COUNTERS("thread_1_operations", 42.0);
    RSM_MARK_COUNTERS("RAM [MB]", 3.1, "cpu utilization", 62.0);
  }}.join();

  std::thread{[]() {
    RSM_MARK_COUNTERS("RAM [MB]", 3.3, "cpu utilization", 52.0);
    RSM_MARK_COUNTERS("thread_2_operations", 85.3);
  }}.join();

  RSM_MARK_COUNTERS("RAM [MB]", 1.0, "cpu utilization", 67.0);

  RSM_flush_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_all_collected_events(rsm::output::format::chrome_trace, filename);

  return 0;
}
