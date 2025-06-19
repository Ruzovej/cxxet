#include <thread>

#include "cxxet/all.hxx"

static void improper_cxxet_usage() {
  // incorrect, because `CXXET_sink_thread_reserve(...)` should have been
  // called:
  CXXET_sink_thread_flush();
}

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout",
                          true); // whatever, in this example ...

  std::thread t{improper_cxxet_usage};

  improper_cxxet_usage();

  t.join();

  return 0;
}
