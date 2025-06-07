#pragma once

#include <mutex>

#include "impl/sink.hpp"
#include "impl/sink_properties.hpp"

namespace cxxst::impl {

struct central_sink : sink {
  explicit central_sink(sink_properties const &aTraits);
  ~central_sink() noexcept override;

  void flush() noexcept;

  void drain(sink &other) noexcept override final;

  [[nodiscard]] sink_properties const &get_traits() const noexcept {
    return traits;
  }

private:
  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  std::mutex mtx;
  long long const time_point;
  sink_properties const &traits;
};

} // namespace cxxst::impl
