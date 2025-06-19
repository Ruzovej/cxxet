#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  CXXET_sink_thread_reserve();

  CXXET_mark_instant("main thread beginning");

  {
    CXXET_mark_complete("main thread, local scope");

    // Unfortunately, using non-default scope (== `cxxet::scope_t::thread`),
    // makes `chrome` & ui.perfetto.dev display it somehow unusably ...

    std::thread t1{[]() {
      CXXET_sink_thread_reserve();
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      CXXET_mark_complete("thread 1");
      CXXET_mark_instant("thread 1 started");
    }};

    std::thread t2{[]() {
      CXXET_sink_thread_reserve();
      CXXET_mark_complete("thread 2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      CXXET_mark_instant("thread 2 started");
    }};

    std::thread t3{[]() {
      CXXET_sink_thread_reserve();
      CXXET_mark_complete("thread 3");
      CXXET_mark_instant("thread 3 started");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }};

    t1.join();
    t2.join();
    t3.join();
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_instant("main terminating");

  return 0;
}
