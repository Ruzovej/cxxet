#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  CXXET_sink_thread_reserve();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counters("RAM [MB]", 1.1, "cpu utilization", 42.0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counters("RAM [MB]", 2.2, "cpu utilization", 22.0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  std::thread t1{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_counters("thread 1 operations", 42.0);
    CXXET_mark_counters("RAM [MB]", 3.1, "cpu utilization", 62.0);
  }};

  std::thread t2{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_counters("RAM [MB]", 3.3, "cpu utilization", 52.0);
    CXXET_mark_counters("thread 2 operations", 85.3);
  }};

  t1.join();
  t2.join();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counters("RAM [MB]", 1.0, "cpu utilization", 67.0);

  return 0;
}
