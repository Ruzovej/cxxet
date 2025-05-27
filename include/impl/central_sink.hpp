#pragma once

#include <mutex>

#include "impl/event/list/list.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

struct central_sink {
  explicit central_sink(bool const silent = true);
  ~central_sink() noexcept;

  void flush();

  void drain(event::list &aEvents);

  central_sink &set_target_filename(char const *const filename) noexcept {
    target_filename = filename;
    return *this;
  }

  central_sink &set_target_format(output::format const fmt) noexcept {
    target_format = fmt;
    return *this;
  }

private:
  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  std::mutex mtx;
  long long const time_point;
  const char *target_filename{nullptr}; // `== nullptr` => no-op
  output::format target_format{output::format::chrome_trace};

protected: // because of testing ...
  event::list events;
};

} // namespace rsm::impl
