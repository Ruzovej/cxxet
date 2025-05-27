#pragma once

#include <mutex>

#include "impl/event/list/list.hpp"
#include "impl/sink_traits.hpp"

namespace rsm::impl {

struct central_sink {
  explicit central_sink(sink_traits const &aTraits);
  ~central_sink() noexcept;

  void flush();

  void drain(event::list &aEvents);

  [[nodiscard]] sink_traits const &get_traits() const noexcept {
    return traits;
  }

private:
  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  std::mutex mtx;
  long long const time_point;
  sink_traits const &traits;

protected: // because of testing ...
  event::list events;
};

} // namespace rsm::impl
