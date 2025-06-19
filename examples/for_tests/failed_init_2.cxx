#include "cxxet/all.hxx"

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout",
                          true); // whatever, in this example ...

  // incorrect, because `CXXET_sink_thread_reserve(...)` should have been
  // called:
  CXXET_mark_complete("will crash on exit ...");

  return 0;
}
