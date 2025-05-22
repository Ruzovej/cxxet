#pragma once

#include "impl/event/any.hpp"
#include "impl/event/list/list.hpp"

namespace rsm::impl {

struct sink {
  explicit sink(sink *aParent, int const default_list_node_capacity = 64);
  virtual ~sink() noexcept;

  inline void append_event(event::any const &evt) noexcept {
    events.append(evt);
  }

  virtual void flush() {
    if (parent && !events.empty()) {
      parent->drain(*this);
    }
  };

  void initialize(sink *aParent,
                  int const default_list_node_capacity = 64) noexcept;

  [[nodiscard]] bool empty() const noexcept { return events.empty(); }

protected:
  virtual void drain(sink &other) {
    events.drain_and_prepend_other(other.events);
  }

  sink(sink const &) = delete;
  sink &operator=(sink const &) = delete;
  sink(sink &&) = delete;
  sink &operator=(sink &&) = delete;

  sink *parent{nullptr};
  event::list events;
};

} // namespace rsm::impl
