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

#include <cassert>

#include <new>
#include <type_traits>
#include <utility>

#include "impl/event/any.hxx"

namespace cxxet::impl::event {

struct list {
  union raw_element;

  struct meta_info {
    constexpr explicit meta_info(long long const aThread_id,
                                 int const aCapacity) noexcept
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

    constexpr raw_element() noexcept : meta{0, 0} {}

    constexpr long long get_thread_id() const noexcept {
      return meta.thread_id;
    }

    constexpr raw_element const *next_node() const noexcept {
      return meta.next;
    }
    constexpr raw_element *&next_node() noexcept { return meta.next; }

    constexpr int get_size() const noexcept { return meta.size; }
    constexpr int get_capacity() const noexcept { return meta.capacity; }

    constexpr int get_next_free_index() noexcept { return meta.size++; }

    constexpr int get_free_capacity() const noexcept {
      return meta.capacity - meta.size;
    }
  };

  struct detailed_event {
    long long thread_id;
    any event;
  };

  class const_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = detailed_event const;
    using pointer = detailed_event const *;
    using reference = detailed_event const &;
    using iterator_category = std::forward_iterator_tag;

    constexpr explicit const_iterator(raw_element *aNode) noexcept
        : node{get_first_valid_and_nonempty_or_nullptr(aNode)} {}

    value_type operator*() const noexcept {
      assert(node);
      assert(index < node->get_size());
      return value_type{node->get_thread_id(), node[index + 1].evt};
    }

    constexpr const_iterator &operator++() noexcept {
      if (index + 1 < node->get_size()) {
        ++index;
      } else {
        node = get_first_valid_and_nonempty_or_nullptr(node->next_node());
        index = 0;
      }
      return *this;
    }

    constexpr const_iterator operator++(int) noexcept {
      auto const ret{*this};
      ++(*this);
      return ret;
    }

    constexpr bool operator==(const_iterator const &other) const noexcept {
      return (node == other.node) && (index == other.index);
    }

    constexpr bool operator!=(const_iterator const &other) const noexcept {
      return !(*this == other);
    }

  private:
    static constexpr raw_element const *
    get_first_valid_and_nonempty_or_nullptr(raw_element const *node) noexcept {
      while ((node != nullptr) && (node->get_size() == 0)) {
        node = node->next_node();
      }
      return node;
    }

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

  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator{first};
  }

  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return const_iterator{nullptr};
  }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return begin();
  }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

private:
  list(list const &) = delete;
  list &operator=(list const &) = delete;
  list(list &&) = delete;
  list &operator=(list &&) = delete;

  raw_element *first{nullptr}, *last{nullptr};
};

} // namespace cxxet::impl::event
