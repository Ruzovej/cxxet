#include <chrono>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_duration.hpp"
#include "rsm/mark_duration_begin.hpp"
#include "rsm/mark_duration_end.hpp"

int main(int argc, char const **argv) {
  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_global_sink(rsm::output::format::chrome_trace, filename, true);

  RSM_init_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    RSM_mark_duration("RAII duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_mark_duration_begin("manual duration test");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_mark_duration_end();

  RSM_mark_duration_begin("main - spawning threads");
  std::thread t1{[]() {
    RSM_init_thread_local_sink();
    RSM_mark_duration("RAII thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
  std::thread t2{[]() {
    RSM_init_thread_local_sink();
    RSM_mark_duration_begin("manual thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    RSM_mark_duration_end();
  }};
  RSM_mark_duration_end();

  // Test overlapping durations:
  {
    RSM_mark_duration("RAII outer duration");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    {
      RSM_mark_duration("RAII inner duration");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_mark_duration_begin("manual outer duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_mark_duration_begin("manual inner duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_mark_duration_end();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_mark_duration_end();

  constexpr int pyramid_height{6};
  for (int i{0}; i < pyramid_height; ++i) {
    RSM_mark_duration_begin("Pyramid level");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for (int i{0}; i < pyramid_height; ++i) {
    RSM_mark_duration_end();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_mark_duration_begin("main - joining threads");
  t1.join();
  t2.join();
  RSM_mark_duration_end();

  return 0;
}
