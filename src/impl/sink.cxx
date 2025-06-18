#include "impl/sink.hxx"

#include <iostream>

#include "impl/dump_records.hxx"

namespace cxxst::impl {

sink::sink() noexcept = default;

sink::~sink() noexcept = default;

void sink::flush_to_file(long long const time_point_zero,
                         cxxst::output::format const fmt,
                         char const *const filename) noexcept {
  if (!events.empty()) {
    try {
      if (filename) {
        // is `time_point_zero` needed?!
        dump_records(events, time_point_zero, fmt, filename);
      }
      events.destroy();
    } catch (std::exception const &e) {
      std::cerr << "Failed to dump records: " << e.what() << '\n';
    }
  }
}

void sink::drain(sink &other) noexcept {
  if (!other.events.empty()) {
    events.drain_other(other.events);
  }
}

} // namespace cxxst::impl
