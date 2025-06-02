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
    RSM_MARK_DURATION("RAII duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_MARK_DURATION_BEGIN("manual duration test");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_MARK_DURATION_END();

  RSM_MARK_DURATION_BEGIN("main - spawning threads");
  std::thread t1{[]() {
    RSM_init_thread_local_sink();
    RSM_MARK_DURATION("RAII thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }};
  std::thread t2{[]() {
    RSM_init_thread_local_sink();
    RSM_MARK_DURATION_BEGIN("manual thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    RSM_MARK_DURATION_END();
  }};
  RSM_MARK_DURATION_END();

  // Test overlapping durations:
  {
    RSM_MARK_DURATION("RAII outer duration");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    {
      RSM_MARK_DURATION("RAII inner duration");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_MARK_DURATION_BEGIN("manual outer duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_MARK_DURATION_BEGIN("manual inner duration");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_MARK_DURATION_END();
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_MARK_DURATION_END();

  constexpr int pyramid_height{6};
  for (int i{0}; i < pyramid_height; ++i) {
    RSM_MARK_DURATION_BEGIN("Pyramid level");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  for (int i{0}; i < pyramid_height; ++i) {
    RSM_MARK_DURATION_END();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_MARK_DURATION_BEGIN("main - joining threads");
  t1.join();
  t2.join();
  RSM_MARK_DURATION_END();

  return 0;
}
