#include <thread>

#include "cxxst/all.hpp"

namespace {

void record_some_events() {
  CXXST_thread_local_sink_reserve();

  CXXST_mark_complete("a complete event that disappears");

  CXXST_mark_instant("event that will never be seen");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXST_mark_counters("lost counter", 42.0);

  CXXST_sink_thread_flush();
}

} // namespace

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  record_some_events();

  CXXST_sink_global_flush(
      cxxst::output::format::chrome_trace,
      nullptr, // file won't be created/overwritten - `nullptr` means to discard
               // all recorded events ...
      true);

  return 0;
}
