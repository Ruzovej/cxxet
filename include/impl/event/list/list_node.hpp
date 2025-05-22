#pragma once

#include <new>

#include "impl/event/any.hpp"

namespace rsm::impl::event::list_node {

union raw_element;

struct meta_info {
  long long thread_id;
  raw_element *next;
  int size;
  int capacity;

  constexpr int get_free_capacity() const noexcept { return capacity - size; }
};

union raw_element {
  meta_info meta;
  any evt;
};

struct handler {
  handler() noexcept;
  ~handler() noexcept;

  handler &set_default_capacity(int const capacity) noexcept {
    default_capacity = capacity;
    return *this;
  }
  [[nodiscard]] int get_default_capacity() const noexcept {
    return default_capacity;
  }

  handler &reserve(bool const force = false) noexcept {
    do_reserve(force);
    return *this;
  }

  void append(any const &event) noexcept {
    if (get_current_free_capacity() < 1) {
      do_reserve(true);
    }
    new (&last[1 + last[0].meta.size++].evt) any{event};
  }

  void drain_other(handler &other) noexcept;

  template <typename callable_t> long long apply(callable_t &&callable) const {
    long long cnt{0};
    auto const pid{get_pid()};
    for (auto it{first}; it != nullptr; it = it[0].meta.next) {
      for (int i{0}; i < it[0].meta.size; ++i) {
        callable(pid, it[0].meta.thread_id, it[i + 1].evt);
        ++cnt;
      }
    }
    return cnt;
  }

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] long long size() const noexcept;

private:
  handler(handler const &) = delete;
  handler &operator=(handler const &) = delete;
  handler(handler &&) = delete;
  handler &operator=(handler &&) = delete;

  int get_current_free_capacity() const noexcept {
    return last ? (last[0].meta.get_free_capacity()) : 0;
  }

  long long get_pid() const noexcept;

  void do_reserve(bool const force) noexcept;

  raw_element *first{nullptr}, *last{nullptr};
  int default_capacity{64};
};

} // namespace rsm::impl::event::list_node
