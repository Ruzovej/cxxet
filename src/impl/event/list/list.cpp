#include "impl/event/list/list.hpp"

#include <unistd.h>

#include <cassert>

#include <type_traits>
#include <utility>

namespace rsm::impl::event {

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
}

void list::append(any const &event) noexcept {
  if (get_current_free_capacity() < 1) {
    reserve(true);
  }
  new (&last[1 + last[0].meta.size++].evt) any{event};
}

void list::set_default_node_capacity(int const capacity) noexcept {
  assert(capacity > 0);
  default_capacity = capacity;
}

void list::reserve(bool const force) noexcept {
  if (force || (get_current_free_capacity() < default_capacity)) {
    auto target{first ? &last[0].meta.next : &first};
    *target = allocate_raw_node_elems(default_capacity);
    last = *target;
  }
}

void list::drain_other(list &other) noexcept {
  assert(this != &other && "attempting to drain and append to self!");
  assert((first == nullptr) == (last == nullptr));
  assert((other.first == nullptr) == (other.last == nullptr));
  if (last) {
    last[0].meta.next = std::exchange(other.first, nullptr);
    last = std::exchange(other.last, nullptr);
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

} // namespace rsm::impl::event

#ifdef CXXST_WITH_UNIT_TESTS

#include <doctest/doctest.h>

namespace rsm::impl {

TEST_CASE("event::list") {
  event::list l;
  long long n;

  SUBCASE("empty") {
    SUBCASE("default") {
      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("after reserve() with default capacity") {
      l.reserve();
      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("after reserve(1) = non-default capacity") {
      l.set_default_node_capacity(1);
      l.reserve();

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
    }

    REQUIRE_EQ(n, 0);
    REQUIRE(l.empty());
    REQUIRE(l.size() == 0);
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
      l.append(a[0]);

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[0]);
        counter++;
      });

      REQUIRE_EQ(counter, 1);
    }

    SUBCASE("after reserve() with default capacity") {
      l.reserve();
      l.append(a[0]);
      l.append(a[1]);

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("after reserve(2) = non-default capacity") {
      l.set_default_node_capacity(2);
      l.reserve();
      for (auto const &evt : a) {
        l.append(evt);
      }

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE_EQ(counter, size);
    }

    SUBCASE("drain other") {
      event::list other;
      other.set_default_node_capacity(2);
      other.reserve();
      other.append(a[0]);
      other.append(a[1]);

      l.drain_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("drain other empty") {
      event::list other;
      l.append(a[0]);
      l.append(a[1]);

      l.drain_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("drain") {
      l.set_default_node_capacity(3); // will have one space left
      l.reserve();
      l.append(a[0]);
      l.append(a[1]);
      event::list other;
      other.set_default_node_capacity(2); // will have to allocate twice
      other.reserve();
      other.append(a[2]);
      other.append(a[3]);
      other.append(a[4]);

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

} // namespace rsm::impl

#endif
