#include <chrono>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_counter.hpp"

int main(int argc, char const **argv) {
  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_global_sink(rsm::output::format::chrome_trace, filename, true);

  RSM_init_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  RSM_MARK_COUNTERS("RAM [MB]", 1.1, "cpu utilization", 42.0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  RSM_MARK_COUNTERS("RAM [MB]", 2.2, "cpu utilization", 22.0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  std::thread t1{[]() {
    RSM_MARK_COUNTERS("thread 1 operations", 42.0);
    RSM_MARK_COUNTERS("RAM [MB]", 3.1, "cpu utilization", 62.0);
  }};

  std::thread t2{[]() {
    RSM_MARK_COUNTERS("RAM [MB]", 3.3, "cpu utilization", 52.0);
    RSM_MARK_COUNTERS("thread 2 operations", 85.3);
  }};

  t1.join();
  t2.join();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  RSM_MARK_COUNTERS("RAM [MB]", 1.0, "cpu utilization", 67.0);

  return 0;
}
