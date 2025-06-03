#include "impl/central_sink.hpp"

#include <iostream>

#include "cxxst/timepoint.hpp"
#include "impl/dump_records.hpp"

namespace cxxst::impl {

central_sink::central_sink(sink_properties const &aTraits)
    : time_point{as_int_ns(now())}, traits{aTraits} {}

central_sink::~central_sink() noexcept { flush(); }

void central_sink::flush() noexcept {
  std::lock_guard lck{mtx};
  if (!events.empty()) {
    try {
      if (traits.target_filename) {
        dump_records(events, time_point, traits.target_format,
                     traits.target_filename);
      }
      events.destroy();
    } catch (std::exception const &e) {
      std::cerr << "Failed to dump records: " << e.what() << '\n';
    }
  }
}

void central_sink::drain(event::list &aEvents) noexcept {
  std::lock_guard lck{mtx};
  events.drain_other(aEvents);
}

} // namespace cxxst::impl
