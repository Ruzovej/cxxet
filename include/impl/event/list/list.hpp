#pragma once

#include "impl/event/any.hpp"
#include "impl/event/list/list_node.hpp"

#include <type_traits>
#include <utility>

namespace rsm::impl::event {

struct list {
  list() = default;
  ~list() noexcept = default;

  void append(any const &event) noexcept { handler.append(event); }

  template <typename callable_t> long long apply(callable_t &&callable) const {
    static_assert(std::is_invocable_r_v<void, // return type
                                        callable_t,
                                        // arguments:
                                        long long const, // process id
                                        long long const, // thread id
                                        any const &>);
    return handler.apply(std::forward<callable_t>(callable));
  }

  void set_default_node_capacity(int const capacity) noexcept {
    handler.set_default_capacity(capacity);
  }

  void reserve(bool const force = false) noexcept { handler.reserve(force); }

  void drain_and_prepend_other(list &other) noexcept {
    handler.drain_and_prepend_other(other.handler);
  }

  [[nodiscard]] bool empty() const noexcept { return handler.empty(); }

  [[nodiscard]] long long size() const noexcept { return handler.size(); }

private:
  list_node::handler handler; // TODO this "intermediate step" isn't needed,
                              // merge `list_node::handler` and `list` ...
};

} // namespace rsm::impl::event
