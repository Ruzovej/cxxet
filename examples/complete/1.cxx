#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

static void pyramid(int const level) {
  CXXET_mark_complete(__FUNCTION__);
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (level > 0) {
    pyramid(level - 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main(int argc, char const **argv) {
  CXXET_sink_thread_reserve();
  CXXET_mark_complete(__FUNCTION__);

  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  std::thread t1{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_complete("scope 1.1");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      CXXET_mark_complete("scope 2.1");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      CXXET_mark_complete("scope 2.2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};

  std::thread t2{[]() {
    CXXET_sink_thread_reserve();
    pyramid(4);
  }};

  pyramid(3);

  t1.join();
  t2.join();

  return 0;
}
