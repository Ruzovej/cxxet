#include <chrono>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_complete.hpp"

static void pyramid(int const level) {
  RSM_MARK_COMPLETE(__FUNCTION__);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (level > 0) {
    pyramid(level - 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main(int argc, char const **argv) {
  RSM_init_thread_local_sink();
  RSM_MARK_COMPLETE(__FUNCTION__);

  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_global_sink(rsm::output::format::chrome_trace, filename, true);

  std::thread t1{[]() {
    RSM_init_thread_local_sink();
    RSM_MARK_COMPLETE("scope 1.1");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      RSM_MARK_COMPLETE("scope 2.1");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      RSM_MARK_COMPLETE("scope 2.2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};

  std::thread t2{[]() {
    RSM_init_thread_local_sink();
    pyramid(4);
  }};

  pyramid(3);

  t1.join();
  t2.join();

  return 0;
}
