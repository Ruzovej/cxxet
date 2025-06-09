#include <chrono>
#include <thread>

#include "cxxst/all.hpp"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXST_sink_global_flush(cxxst::output::format::chrome_trace, filename, true);

  CXXST_sink_thread_reserve();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    CXXST_mark_duration("RAII duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXST_mark_duration_begin("manual duration test");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXST_mark_duration_end();

  CXXST_mark_duration_begin("main - spawning threads");
  std::thread t1{[]() {
    CXXST_sink_thread_reserve();
    CXXST_mark_duration("RAII thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
  std::thread t2{[]() {
    CXXST_sink_thread_reserve();
    CXXST_mark_duration_begin("manual thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    CXXST_mark_duration_end();
  }};
  CXXST_mark_duration_end();

  // Test overlapping durations:
  {
    CXXST_mark_duration("RAII outer duration");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    {
      CXXST_mark_duration("RAII inner duration");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXST_mark_duration_begin("manual outer duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXST_mark_duration_begin("manual inner duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXST_mark_duration_end();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXST_mark_duration_end();

  constexpr int pyramid_height{6};
  for (int i{0}; i < pyramid_height; ++i) {
    CXXST_mark_duration_begin("Pyramid level");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for (int i{0}; i < pyramid_height; ++i) {
    CXXST_mark_duration_end();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXST_mark_duration_begin("main - joining threads");
  t1.join();
  t2.join();
  CXXST_mark_duration_end();

  return 0;
}
