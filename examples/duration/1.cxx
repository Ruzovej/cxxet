#include <chrono>
#include <thread>

#include "cxxet/all.hxx"

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  CXXET_sink_thread_reserve();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    CXXET_mark_duration("RAII duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_mark_duration_begin("manual duration test");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_end();

  CXXET_mark_duration_begin("main - spawning threads");
  std::thread t1{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_duration("RAII thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
  std::thread t2{[]() {
    CXXET_sink_thread_reserve();
    CXXET_mark_duration_begin("manual thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    CXXET_mark_duration_end();
  }};
  CXXET_mark_duration_end();

  // Test overlapping durations:
  {
    CXXET_mark_duration("RAII outer duration");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    {
      CXXET_mark_duration("RAII inner duration");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_mark_duration_begin("manual outer duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_begin("manual inner duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_end();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  CXXET_mark_duration_end();

  constexpr int pyramid_height{6};
  for (int i{0}; i < pyramid_height; ++i) {
    CXXET_mark_duration_begin("Pyramid level");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for (int i{0}; i < pyramid_height; ++i) {
    CXXET_mark_duration_end();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  CXXET_mark_duration_begin("main - joining threads");
  t1.join();
  t2.join();
  CXXET_mark_duration_end();

  return 0;
}
