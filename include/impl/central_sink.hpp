#pragma once

#include <mutex>

#include "impl/sink.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

struct central_sink : sink {
  explicit central_sink(bool const silent = true);
  ~central_sink() noexcept override;

  void flush();

  void drain(sink &other) override;

  central_sink &set_target_filename(char const *const filename) noexcept {
    target_filename = filename;
    return *this;
  }

  central_sink &set_target_format(output::format const fmt) noexcept {
    target_format = fmt;
    return *this;
  }

private:
  std::mutex mtx;
  long long const time_point;
  const char *target_filename{nullptr}; // `== nullptr` => no-op
  output::format target_format{output::format::chrome_trace};
};

} // namespace rsm::impl
