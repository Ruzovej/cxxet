#include "cxxst/all.hxx"

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXST_sink_thread_reserve();

  CXXST_mark_complete("main");

  CXXST_mark_instant("instant");

  CXXST_mark_counters("counter", 42.0);

  return 0;
}
