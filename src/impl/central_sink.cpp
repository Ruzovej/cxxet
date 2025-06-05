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
  flush_to_file(time_point, traits.target_format, traits.target_filename);
}

void central_sink::drain(sink &other) noexcept {
  std::lock_guard lck{mtx};
  sink::drain(other);
}

} // namespace cxxst::impl
