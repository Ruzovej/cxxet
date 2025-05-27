#pragma once

#include "impl/central_sink.hpp"
#include "impl/event/list/list.hpp"

namespace rsm::impl {

struct local_sink {
  explicit local_sink(central_sink &aParent);
  ~local_sink() noexcept;

  inline void append_event(event::any const &evt) noexcept {
    events.append(evt);
  }

  void flush();

  void reserve();

private:
  local_sink(local_sink const &) = delete;
  local_sink &operator=(local_sink const &) = delete;
  local_sink(local_sink &&) = delete;
  local_sink &operator=(local_sink &&) = delete;

  central_sink &parent;

protected: // because of testing ...
  event::list events;
};

} // namespace rsm::impl
