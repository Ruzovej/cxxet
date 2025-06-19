#include <thread>

#include "cxxet/all.hxx"

namespace {

void record_some_events() {
  CXXET_sink_thread_reserve();

  CXXET_mark_complete("a complete event that disappears");

  CXXET_mark_instant("event that will never be seen");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counters("lost counter", 42.0);

  CXXET_sink_thread_flush();
}

} // namespace

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  record_some_events();

  CXXET_sink_global_flush(
      cxxet::output::format::chrome_trace,
      nullptr, // file won't be created/overwritten - `nullptr` means to discard
               // all recorded events ...
      true);

  return 0;
}
