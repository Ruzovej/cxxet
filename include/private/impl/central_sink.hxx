#pragma once

#include <mutex>

#include "cxxet/output_format.hxx"
#include "impl/sink.hxx"
#include "impl/sink_properties.hxx"

namespace cxxet::impl {

struct central_sink : sink {
  explicit central_sink(sink_properties const &traits);
  ~central_sink() noexcept override;

  void flush(output::format const aFmt, char const *const aFilename,
             bool const defer) noexcept;

  void drain(sink &other) noexcept override final;

private:
  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  void do_flush() noexcept;

  std::mutex mtx;
  long long const time_point;
  output::format fmt;
  char const *target_filename;
};

} // namespace cxxet::impl
