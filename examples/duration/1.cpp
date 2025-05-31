#include <chrono>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_duration.hpp"
#include "rsm/mark_duration_begin.hpp"
#include "rsm/mark_duration_end.hpp"

int main(int argc, char const **argv) {
  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_all_collected_events(rsm::output::format::chrome_trace, filename,
                                 true);

  RSM_init_thread_local_sink();

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    RSM_MARK_DURATION("RAII duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  RSM_MARK_DURATION_BEGIN("manual duration test");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  RSM_MARK_DURATION_END();

  std::thread{[]() {
    RSM_init_thread_local_sink();
    RSM_MARK_DURATION("RAII thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }}.join();

  std::thread{[]() {
    RSM_init_thread_local_sink();
    RSM_MARK_DURATION_BEGIN("manual thread duration test");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    RSM_MARK_DURATION_END();
  }}.join();

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

  return 0;
}
