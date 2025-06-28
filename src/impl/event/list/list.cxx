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

#include "impl/event/list/list.hxx"

#include <unistd.h>

#include <cassert>

namespace cxxet::impl::event {

namespace {

list::raw_element *allocate_raw_node_elems(int const capacity) noexcept {
  // this can throw ... but if it does, it means allocation failed -> how to
  // handle it? Let's just crash ...
  auto *data{new list::raw_element[static_cast<unsigned>(capacity) + 1]};

  new (&data[0].meta)
      list::meta_info{static_cast<long long>(gettid()), nullptr, 0, capacity};

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
    auto *next{first->meta.next};
    delete[] first;
    first = next;
  }
  last = nullptr;
}

void list::append(any const &event) noexcept {
  new (&last[1 + last[0].meta.size++].evt) any{event};
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
  if (get_current_free_capacity() < capacity) {
    auto target{first ? &last[0].meta.next : &first};
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
      last[0].meta.next = std::exchange(other.first, nullptr);
      last = std::exchange(other.last, nullptr);
    }
  } else {
    std::swap(first, other.first);
    std::swap(last, other.last);
  }
}

[[nodiscard]] bool list::empty() const noexcept {
  if (first != nullptr) {
    for (auto it{first}; it != nullptr; it = it[0].meta.next) {
      if (it[0].meta.size > 0) {
        return false;
      }
    }
  }
  return true;
}

[[nodiscard]] long long list::size() const noexcept {
  long long sz{0};
  for (auto it{first}; it != nullptr; it = it[0].meta.next) {
    sz += it[0].meta.size;
  }
  return sz;
}

int list::get_current_free_capacity() const noexcept {
  return last ? (last[0].meta.get_free_capacity()) : 0;
}

long long list::get_pid() noexcept { return static_cast<long long>(getpid()); }

} // namespace cxxet::impl::event

#ifdef CXXET_WITH_UNIT_TESTS

#include <doctest/doctest.h>

namespace cxxet::impl {

TEST_CASE("event::list") {
  event::list l;
  long long n;

  SUBCASE("empty") {
    SUBCASE("default") {
      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("after reserve(1)") {
      l.reserve(1);

      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("drain other empty") {
      event::list other;
      l.drain_other(other);

      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });

      REQUIRE(other.empty());
    }

    SUBCASE("drain other empty (but with previous reserve)") {
      event::list other;
      other.reserve(3);
      l.drain_other(other);

      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });

      REQUIRE(other.empty());
    }

    REQUIRE_EQ(n, 0);
    REQUIRE(l.empty());
    REQUIRE_EQ(l.size(), 0);
  }

  SUBCASE("append & apply") {
    int counter{0};

    constexpr int size{5};
    event::any a[size];

    new (&a[0].evt.dur_begin)
        event::duration_begin{51, 52, 53, "test begin", 0};

    new (&a[1].evt.dur_end) event::duration_end{1, 2, 3, "test end", 5};

    new (&a[2].evt.cmpl) event::complete{4, 5, 6, "test complete", 10, 15};

    new (&a[3].evt.cntr) event::counter{7, 8, 9, "test counter", 20, 42.666};

    new (&a[4].evt.inst) event::instant{
        45, 78, 89, "test instant", scope_t::thread, 'R', 32109, 2'000'000'001,
        25};

    SUBCASE("without reserve()") {
      REQUIRE_EQ(l.get_current_free_capacity(), 0);
      l.safe_append(a[0], 5);
      REQUIRE_EQ(l.get_current_free_capacity(), 4);

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[0]);
        counter++;
      });

      REQUIRE_EQ(counter, 1);
    }

    SUBCASE("after reserve()") {
      l.reserve(2);
      for (auto const &evt : a) {
        l.safe_append(evt, 1);
      }

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE_EQ(counter, size);
    }

    SUBCASE("drain other") {
      l.safe_append(a[0], 3);
      event::list other;
      other.reserve(2);
      other.append(a[1]);

      l.drain_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("drain other empty (two times)") {
      l.safe_append(a[0], 6);
      event::list other;

      l.drain_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 1);

      l.drain_other(other); // drain second time ...

      counter = 0;
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 1);
    }

    SUBCASE("drain") {
      l.reserve(3);
      REQUIRE_EQ(l.get_current_free_capacity(), 3);
      l.append(a[0]);
      REQUIRE_EQ(l.get_current_free_capacity(), 2);
      l.append(a[1]);
      REQUIRE_EQ(l.get_current_free_capacity(), 1);

      event::list other;
      // will have to allocate twice (once here, and once later):
      other.reserve(2);
      other.append(a[2]);
      REQUIRE_EQ(other.get_current_free_capacity(), 1);
      other.append(a[3]);
      REQUIRE_EQ(other.get_current_free_capacity(), 0);
      other.safe_append(a[4], 4);
      REQUIRE(other.get_current_free_capacity() >= 3);

      l.drain_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, size);
    }

    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(l.size(), n);
    REQUIRE(!l.empty());
  }
}

} // namespace cxxet::impl

#endif
