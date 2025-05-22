#pragma once

#include "impl/event/any.hpp"
#include "impl/event/list/list.hpp"

namespace rsm::impl {

struct sink {
  sink();
  virtual ~sink() noexcept;

  virtual void drain(sink &other) { events.drain_other(other.events); }

  [[nodiscard]] bool empty() const noexcept { return events.empty(); }

  void set_default_list_node_capacity(int const capacity) noexcept {
    events.set_default_node_capacity(capacity);
  }
  [[nodiscard]] int get_default_capacity() const noexcept {
    return events.get_default_capacity();
  }

protected:
  sink(sink const &) = delete;
  sink &operator=(sink const &) = delete;
  sink(sink &&) = delete;
  sink &operator=(sink &&) = delete;

  event::list events;
};

} // namespace rsm::impl
