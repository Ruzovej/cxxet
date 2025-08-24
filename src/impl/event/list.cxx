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

#include "impl/event/list.hxx"

#include <unistd.h>

#include <cassert>

namespace cxxet::impl::event {

namespace {

list::raw_element *allocate_raw_node_elems(int const capacity) noexcept {
  assert(capacity > 0);
  // this can throw ... but if it does, it means allocation failed -> how to
  // handle it? Let's just crash ...
  auto *data{new list::raw_element[static_cast<unsigned>(capacity) + 1]};

  new (&data[0].meta)
      list::meta_info{static_cast<long long>(gettid()), capacity};

  return data;
}

} // namespace

list::list() noexcept = default;

list::~list() noexcept {
  static_assert(std::is_trivially_destructible_v<list::meta_info>);
  static_assert(std::is_trivially_destructible_v<list::raw_element>);
  destroy();
}

void list::destroy() noexcept {
  while (first) {
    auto *const next{first->next_node()};
    delete[] first;
    first = next;
  }
  last = nullptr;
}

void list::append(any const &event) noexcept {
  new (&last[1 + last->get_next_free_index()].evt) any{event};
}

void list::safe_append(any const &event, int const node_capacity) noexcept {
  assert(node_capacity > 0 && "node capacity must be positive!");
  if (!has_free_capacity(1)) {
    reserve(node_capacity);
  }
  append(event);
}

bool list::has_free_capacity(int const capacity) const noexcept {
  assert(capacity > 0 && "node capacity must be positive!");
  return get_current_free_capacity() >= capacity;
}

void list::reserve(int const capacity) noexcept {
  if (!has_free_capacity(capacity)) {
    auto target{first ? &last->next_node() : &first};
    *target = allocate_raw_node_elems(capacity);
    last = *target;
  }
}

void list::drain_other(list &other) noexcept {
  assert(this != &other && "attempting to drain and append to self!");
  assert((first == nullptr) == (last == nullptr));
  assert((other.first == nullptr) == (other.last == nullptr));
  if (last) {
    if (other.last) {
      last->next_node() = std::exchange(other.first, nullptr);
      last = std::exchange(other.last, nullptr);
    }
  } else {
    std::swap(first, other.first);
    std::swap(last, other.last);
  }
}

[[nodiscard]] bool list::empty() const noexcept {
  if (first != nullptr) {
    for (auto it{first}; it != nullptr; it = it->next_node()) {
      if (it->get_size() > 0) {
        return false;
      }
    }
  }
  return true;
}

[[nodiscard]] long long list::size() const noexcept {
  long long sz{0};
  for (auto it{first}; it != nullptr; it = it->next_node()) {
    sz += it->get_size();
  }
  return sz;
}

int list::get_current_free_capacity() const noexcept {
  return last ? (last->get_free_capacity()) : 0;
}

} // namespace cxxet::impl::event

#ifdef CXXET_WITH_UNIT_TESTS

#include <algorithm>
#include <array>

#include <doctest/doctest.h>

namespace cxxet::impl {

namespace {

TEST_CASE("event::list") {
  event::list l;

  SUBCASE("empty") {
    SUBCASE("default") {
      // nothing ...
    }

    SUBCASE("after reserve(1)") {
      // force 2 lines ...
      l.reserve(1);
    }

    SUBCASE("drain other empty") {
      event::list other;
      REQUIRE(other.empty());

      l.drain_other(other);

      REQUIRE(other.empty());
    }

    SUBCASE("drain other empty (but with previous reserve)") {
      event::list other;
      other.reserve(3);
      REQUIRE(other.empty());

      l.drain_other(other);

      REQUIRE(other.empty());
    }

    REQUIRE_EQ(std::distance(l.cbegin(), l.cend()), 0);
    REQUIRE(l.empty());
    REQUIRE_EQ(l.size(), 0);
  }

  SUBCASE("append & iterate") {
    unsigned cnt{0};

    constexpr int size{7};
    std::array<event::any, size> a;

    new (&a[0].evt.dur_begin)
        event::duration_begin{51, 52, 53, "test begin", 0};

    new (&a[1].evt.dur_end) event::duration_end{1, 2, 3, "test end", 5};

    new (&a[2].evt.cmpl) event::complete{4, 5, 6, "test complete", 10, 15};

    new (&a[3].evt.cntr) event::counter{7, 8, 9, "test counter", 20, 42.666};

    new (&a[4].evt.inst) event::instant{
        45, 78, 89, "test instant", scope_t::thread, 'R', 32109, 2'000'000'001,
        25};

    new (&a[5].evt.meta) event::metadata{"some other metadata str value ...",
                                         event::metadata_type::process_name};

    new (&a[6].evt.meta)
        event::metadata{42'132, event::metadata_type::thread_sort_index};

    SUBCASE("without reserve()") {
      REQUIRE_EQ(l.get_current_free_capacity(), 0);
      l.safe_append(a[0], 5);
      REQUIRE_GE(l.get_current_free_capacity(), 4);

      for (auto const &p : l) {
        REQUIRE_EQ(p.event, a[cnt++]);
      }
      REQUIRE_EQ(cnt, 1);
    }

    SUBCASE("after reserve()") {
      l.reserve(2);
      for (auto const &evt : a) {
        l.safe_append(evt, 1);
      }

      for (auto const &p : l) {
        REQUIRE_EQ(p.event, a[cnt++]);
      }
      REQUIRE_EQ(cnt, size);
    }

    SUBCASE("drain other") {
      l.safe_append(a[0], 3);

      event::list other;
      other.reserve(2);
      other.append(a[1]);

      l.drain_other(other);
      REQUIRE(other.empty());
      REQUIRE(!l.empty());

      for (auto const &p : l) {
        REQUIRE_EQ(p.event, a[cnt++]);
      }
      REQUIRE_EQ(cnt, 2);
    }

    SUBCASE("drain other empty (two times)") {
      event::list other;
      REQUIRE(other.empty());
      other.reserve(3);
      REQUIRE(other.empty());

      l.drain_other(other);
      REQUIRE(other.empty());

      l.safe_append(a[0], 6);

      for (auto const &p : l) {
        REQUIRE_EQ(p.event, a[cnt++]);
      }
      REQUIRE_EQ(cnt, 1);

      other.reserve(5);
      REQUIRE(other.empty());
      l.drain_other(other); // drain second time ...
      REQUIRE(other.empty());

      cnt = 0;
      for (auto const &p : l) {
        REQUIRE_EQ(p.event, a[cnt++]);
      }
      REQUIRE_EQ(cnt, 1);
    }

    SUBCASE("drain") {
      l.reserve(3);
      REQUIRE(l.empty());
      REQUIRE_GE(l.get_current_free_capacity(), 3);
      l.append(a[0]);
      REQUIRE(!l.empty());
      REQUIRE_GE(l.get_current_free_capacity(), 2);
      l.append(a[1]);
      REQUIRE(!l.empty());
      REQUIRE_GE(l.get_current_free_capacity(), 1);

      event::list other;
      // will have to allocate twice (once here, and once later):
      other.reserve(2);
      other.append(a[2]);
      REQUIRE_GE(other.get_current_free_capacity(), 1);
      other.append(a[3]);
      REQUIRE_GE(other.get_current_free_capacity(), 0);
      other.safe_append(a[4], 4);
      REQUIRE_GE(other.get_current_free_capacity(), 3);

      l.drain_other(other);
      REQUIRE(other.empty());
      REQUIRE_EQ(other.get_current_free_capacity(), 0);

      other.safe_append(a[5], 2);
      REQUIRE_GE(other.get_current_free_capacity(), 1);
      other.append(a[6]);
      REQUIRE_GE(other.get_current_free_capacity(), 0);

      l.drain_other(other);
      REQUIRE(other.empty());

      for (auto const &p : l) {
        REQUIRE_EQ(p.event, a[cnt++]);
      }

      REQUIRE(other.empty());
      REQUIRE_EQ(cnt, size);
    }

    REQUIRE_EQ(l.size(), cnt);
    REQUIRE(!l.empty());
  }
}

} // namespace

} // namespace cxxet::impl

#endif
