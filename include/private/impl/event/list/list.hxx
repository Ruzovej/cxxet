/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <new>
#include <type_traits>
#include <utility>

#include "impl/event/any.hxx"

namespace cxxet::impl::event {

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

  void append(any const &event) noexcept;

  void safe_append(any const &event, int const node_capacity) noexcept;

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

  bool has_free_capacity(int const capacity) const noexcept;

  void reserve(int const capacity) noexcept;

  void drain_other(list &other) noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] long long size() const noexcept;

  int get_current_free_capacity() const noexcept;

private:
  list(list const &) = delete;
  list &operator=(list const &) = delete;
  list(list &&) = delete;
  list &operator=(list &&) = delete;

  static long long get_pid() noexcept;

  raw_element *first{nullptr}, *last{nullptr};
};

} // namespace cxxet::impl::event
