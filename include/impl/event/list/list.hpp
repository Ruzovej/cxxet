#pragma once

#include <new>
#include <type_traits>
#include <utility>

#include "impl/event/any.hpp"

namespace rsm::impl::event {

struct list {
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

    constexpr raw_element() noexcept : meta{} {}
  };

  list() noexcept;
  ~list() noexcept;

  void destroy() noexcept;

  void append(any const &event) noexcept {
    if (get_current_free_capacity() < 1) {
      reserve(true);
    }
    new (&last[1 + last[0].meta.size++].evt) any{event};
  }

  template <typename callable_t> long long apply(callable_t &&callable) const {
    static_assert(std::is_invocable_r_v<void, // return type
                                        callable_t,
                                        // arguments:
                                        long long const, // process id
                                        long long const, // thread id
                                        any const &>);
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

  void set_default_node_capacity(int const capacity) noexcept;

  void reserve(bool const force = false) noexcept;

  void drain_other(list &other) noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] long long size() const noexcept;

private:
  list(list const &) = delete;
  list &operator=(list const &) = delete;
  list(list &&) = delete;
  list &operator=(list &&) = delete;

  int get_current_free_capacity() const noexcept {
    return last ? (last[0].meta.get_free_capacity()) : 0;
  }

  static long long get_pid() noexcept;

  raw_element *first{nullptr}, *last{nullptr};
  int default_capacity{64};
};

} // namespace rsm::impl::event
