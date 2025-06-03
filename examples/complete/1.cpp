#include <chrono>
#include <thread>

#include "cxxst/all.hpp"

static void pyramid(int const level) {
  CXXST_mark_complete(__FUNCTION__);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (level > 0) {
    pyramid(level - 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main(int argc, char const **argv) {
  CXXST_init_thread_local_sink();
  CXXST_mark_complete(__FUNCTION__);

  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXST_flush_global_sink(cxxst::output::format::chrome_trace, filename, true);

  std::thread t1{[]() {
    CXXST_init_thread_local_sink();
    CXXST_mark_complete("scope 1.1");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      CXXST_mark_complete("scope 2.1");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      CXXST_mark_complete("scope 2.2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};

  std::thread t2{[]() {
    CXXST_init_thread_local_sink();
    pyramid(4);
  }};

  pyramid(3);

  t1.join();
  t2.join();

  return 0;
}
