#pragma once

#include <mutex>

#include "impl/event/list/list.hpp"
#include "impl/sink_properties.hpp"

namespace cxxst::impl {

struct central_sink {
  explicit central_sink(sink_properties const &aTraits);
  ~central_sink() noexcept;

  void flush() noexcept;

  void drain(event::list &aEvents) noexcept;

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

protected: // because of testing ...
  event::list events;
};

} // namespace cxxst::impl
