#include <thread>

#include "cxxst/all.hpp"

static void improper_cxxst_usage() {
  // incorrect, because `CXXST_sink_thread_reserve(...)` should have been
  // called:
  CXXST_sink_thread_flush();
}

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout",
                          true); // whatever, in this example ...

  std::thread t{improper_cxxst_usage};

  improper_cxxst_usage();

  t.join();

  return 0;
}
