#include <chrono>
#include <thread>

#include "rsm.hpp"
#include "rsm/mark_complete.hpp"

static void pyramid(int const level) {
  RSM_MARK_COMPLETE("pyramid");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  if (level > 0) {
    pyramid(level - 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main(int argc, char const **argv) {
  RSM_MARK_COMPLETE(__FUNCTION__);

  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_all_collected_events(rsm::output::format::chrome_trace, filename,
                                 true);

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  {
    RSM_MARK_COMPLETE("scope 1");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    {
      RSM_MARK_COMPLETE("scope 2");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    {
      RSM_MARK_COMPLETE("scope 3");
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  pyramid(6);

  return 0;
}
