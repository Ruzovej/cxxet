#include "impl/central_sink.hpp"

#include <iostream>

#include "cxxst/timepoint.hpp"
#include "impl/dump_records.hpp"

namespace cxxst::impl {

central_sink::central_sink(sink_properties const &traits)
    : time_point{as_int_ns(now())}, fmt(traits.target_format),
      target_filename(traits.target_filename) {}

central_sink::~central_sink() noexcept {
  std::lock_guard lck{mtx};
  do_flush();
}

void central_sink::flush(output::format const aFmt, char const *const aFilename,
                         bool const defer) noexcept {
  std::lock_guard lck{mtx};
  fmt = aFmt;
  target_filename = aFilename;
  if (!defer) {
    do_flush();
  }
}

void central_sink::drain(sink &other) noexcept {
  std::lock_guard lck{mtx};
  sink::drain(other);
}

void central_sink::do_flush() noexcept {
  flush_to_file(time_point, fmt, target_filename);
  // to avoid flushing again & rewriting the file implicitly ...:
  target_filename = nullptr;
}

} // namespace cxxst::impl
