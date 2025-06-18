#include "cxxst/all.hxx"

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout",
                          true); // whatever, in this example ...

  // incorrect, because `CXXST_sink_thread_reserve(...)` should have been
  // called:
  CXXST_mark_complete("will crash on exit ...");

  return 0;
}
