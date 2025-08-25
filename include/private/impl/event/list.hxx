/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License along
  with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "impl/event/any.hxx"

namespace cxxet::impl::event {

class list {
  union raw_element;

  struct meta_info {
    explicit meta_info(long long const aThread_id, int const aCapacity) noexcept
        : thread_id{aThread_id}, next{nullptr}, size{0}, capacity{aCapacity} {}

    friend union raw_element;

  private:
    long long thread_id;
    raw_element *next;
    int size;
    int capacity;
  };

  union raw_element {
    meta_info meta;
    any evt;

    raw_element() noexcept;

    static raw_element *new_elems(int const capacity) noexcept;
    static void delete_elems(raw_element const *const elems) noexcept;

    long long get_thread_id() const noexcept;

    raw_element const *get_next_node() const noexcept;
    void set_next_node(raw_element *const next) noexcept;

    any const &operator[](int const idx) const noexcept;

    void push_any(any const &event) noexcept;

    int get_size() const noexcept;
    int get_capacity() const noexcept;

    int get_free_capacity() const noexcept;
  };

public:
  struct detailed_event {
    long long const thread_id;
    any const &event;
  };

  class const_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = detailed_event const;
    using pointer = detailed_event const *;
    using reference = detailed_event const &;
    using iterator_category = std::forward_iterator_tag;

    explicit const_iterator(raw_element *aNode) noexcept;

    value_type operator*() const noexcept;

    const_iterator &operator++() noexcept;

    const_iterator operator++(int) noexcept;

    bool operator==(const_iterator const &other) const noexcept;

    bool operator!=(const_iterator const &other) const noexcept;

  private:
    static raw_element const *
    get_first_valid_and_nonempty_or_nullptr(raw_element const *node) noexcept;

    raw_element const *node;
    int index{0};
  };

  list() noexcept;
  ~list() noexcept;

  void destroy() noexcept;

  void append(any const &event) noexcept;

  void safe_append(any const &event, int const node_capacity) noexcept;

  bool has_free_capacity(int const capacity) const noexcept;

  void reserve(int const capacity) noexcept;

  void drain_other(list &other) noexcept;

  [[nodiscard]] bool empty() const noexcept;

  [[nodiscard]] long long size() const noexcept;

  [[nodiscard]] int get_current_free_capacity() const noexcept;

  [[nodiscard]] const_iterator begin() const noexcept {
    return const_iterator{first};
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return const_iterator{nullptr};
  }

  [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] const_iterator cend() const noexcept { return end(); }

private:
  list(list const &) = delete;
  list &operator=(list const &) = delete;
  list(list &&) = delete;
  list &operator=(list &&) = delete;

  raw_element *first{nullptr}, *last{nullptr};
};

} // namespace cxxet::impl::event
