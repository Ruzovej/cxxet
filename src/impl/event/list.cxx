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

#include <cassert>

#include <new>
#include <type_traits>

#include "cxxet/get_thread_id.hxx"

namespace cxxet::impl::event {

list::raw_element::raw_element() noexcept : meta{0, 0} {
  // where else to check this ...:
  static_assert(std::is_trivially_destructible_v<raw_element>);
  static_assert(std::is_trivially_destructible_v<meta_info>);
}

list::raw_element *list::raw_element::new_elems(int const capacity) noexcept {
  assert(capacity > 0);
  // this can throw ... but if it does, it means allocation failed -> how to
  // handle it? Let's just crash ...
  auto *data{new raw_element[static_cast<unsigned>(capacity) + 1]};

  new (&(data->meta)) meta_info{get_thread_id(), capacity};

  return data;
}

void list::raw_element::delete_elems(raw_element const *const elems) noexcept {
  delete[] elems;
}

long long list::raw_element::associated_thread_id() const noexcept {
  return meta.thread_id;
}

list::raw_element const *list::raw_element::get_next_node() const noexcept {
  return meta.next;
}

void list::raw_element::set_next_node(raw_element *const next) noexcept {
  meta.next = next;
}

any const &list::raw_element::operator[](int const idx) const noexcept {
  assert((0 <= idx) && (idx < get_size()));
  return this[1 + idx].evt;
}

void list::raw_element::push_any(any const &event) noexcept {
  assert(0 < get_free_capacity());
  new (&this[++meta.size]) any{event};
}

int list::raw_element::get_size() const noexcept { return meta.size; }

int list::raw_element::get_capacity() const noexcept { return meta.capacity; }

int list::raw_element::get_free_capacity() const noexcept {
  return meta.capacity - meta.size;
}

list::const_iterator::const_iterator(raw_element *aNode) noexcept
    : node{get_first_valid_and_nonempty_or_nullptr(aNode)} {}

list::const_iterator::value_type
list::const_iterator::operator*() const noexcept {
  assert(node);
  return value_type{node->associated_thread_id(), (*node)[index]};
}

list::const_iterator &list::const_iterator::operator++() noexcept {
  if (index + 1 < node->get_size()) {
    ++index;
  } else {
    node = get_first_valid_and_nonempty_or_nullptr(node->get_next_node());
    index = 0;
  }
  return *this;
}

list::const_iterator list::const_iterator::operator++(int) noexcept {
  auto const ret{*this};
  ++(*this);
  return ret;
}

bool list::const_iterator::operator==(
    const_iterator const &other) const noexcept {
  return (node == other.node) && (index == other.index);
}

bool list::const_iterator::operator!=(
    const_iterator const &other) const noexcept {
  return !(*this == other);
}

list::raw_element const *
list::const_iterator::get_first_valid_and_nonempty_or_nullptr(
    raw_element const *node) noexcept {
  while ((node != nullptr) && (node->get_size() == 0)) {
    node = node->get_next_node();
  }
  return node;
}

list::list() noexcept = default;

list::~list() noexcept { destroy(); }

void list::destroy() noexcept {
  auto const *ptr{std::exchange(first, nullptr)};
  while (ptr) {
    auto *const next{ptr->get_next_node()};
    list::raw_element::delete_elems(ptr);
    ptr = next;
  }
  last = nullptr;
}

void list::append(any const &event) noexcept { last->push_any(event); }

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
    assert((first == nullptr) == (last == nullptr));
    auto *const new_elems{raw_element::new_elems(capacity)};
    if (last != nullptr) {
      last->set_next_node(new_elems);
    } else {
      first = new_elems;
    }
    last = new_elems;
  }
}

void list::drain_other(list &other) noexcept {
  assert(this != &other && "attempting to drain and append to self!");
  assert((first == nullptr) == (last == nullptr));
  assert((other.first == nullptr) == (other.last == nullptr));
  if (last) {
    if (other.last) {
      last->set_next_node(std::exchange(other.first, nullptr));
      last = std::exchange(other.last, nullptr);
    }
  } else {
    std::swap(first, other.first);
    std::swap(last, other.last);
  }
}

[[nodiscard]] bool list::empty() const noexcept {
  if (first != nullptr) {
    for (auto const *it{first}; it != nullptr; it = it->get_next_node()) {
      if (it->get_size() > 0) {
        return false;
      }
    }
  }
  return true;
}

[[nodiscard]] long long list::size() const noexcept {
  long long sz{0};
  for (auto const *it{first}; it != nullptr; it = it->get_next_node()) {
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

    new (&a[0].evt.dur_begin) event::duration_begin{
        51, 52, output::category_flag{53}, "test begin", 0};

    new (&a[1].evt.dur_end)
        event::duration_end{1, 2, output::category_flag{3}, "test end", 5};

    new (&a[2].evt.cmpl) event::complete{
        4, 5, output::category_flag{6}, "test complete", 10, 15};

    new (&a[3].evt.cntr) event::counter{
        7, 8, output::category_flag{9}, "test counter", 20, 42.666};

    new (&a[4].evt.inst) event::instant{45,
                                        78,
                                        output::category_flag{89},
                                        "test instant",
                                        scope_t::thread,
                                        'R',
                                        32109,
                                        2'000'000'001,
                                        25};

    new (&a[5].evt.meta) event::metadata{'c',
                                         4553,
                                         output::category_flag{145},
                                         "some other metadata str value ...",
                                         event::metadata_type::process_name,
                                         'D',
                                         9897,
                                         123155};

    new (&a[6].evt.meta)
        event::metadata{'r',
                        134,
                        output::category_flag{13334},
                        42'132,
                        event::metadata_type::thread_sort_index,
                        'Q',
                        899};

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

#ifdef CXXET_WITH_BENCHMARKS

#include <benchmark/benchmark.h>

namespace {

namespace helper {

constexpr long long get_internal_array_size(long long const capacity) noexcept {
  return static_cast<long long>(sizeof(cxxet::impl::event::list::raw_element)) *
         (capacity + 1);
}

void placement_new_evt(void *addr) noexcept {
  new (addr) cxxet::impl::event::complete{cxxet::output::category_flag{123},
                                          "complete", 1, 2};
  benchmark::DoNotOptimize(addr);
}

} // namespace helper

void cxxet_list_raw_element_new_delete(benchmark::State &state) {
  auto const capacity{static_cast<int>(state.range(0))};

  for (auto _ : state) {
    auto *elems{cxxet::impl::event::list::raw_element::new_elems(capacity)};
    benchmark::DoNotOptimize(elems);
    cxxet::impl::event::list::raw_element::delete_elems(elems);
  }

  state.SetBytesProcessed(state.iterations() *
                          helper::get_internal_array_size(capacity));
}
BENCHMARK(cxxet_list_raw_element_new_delete)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(4096)
    ->Arg(32768);

namespace cxxet_alternative {

char *new_raw_elems(int const capacity) noexcept {
  assert(capacity > 0);
  auto *data{new char[static_cast<std::size_t>(
      helper::get_internal_array_size(capacity))]};

  new (data) cxxet::impl::event::list::meta_info{cxxet::impl::get_thread_id(),
                                                 capacity};

  return data;
}

void delete_raw_elems(char *const elems) noexcept { delete[] elems; }

char *malloc_raw_elems(int const capacity) noexcept {
  assert(capacity > 0);
  auto *data{reinterpret_cast<char *>(malloc(
      static_cast<std::size_t>(helper::get_internal_array_size(capacity))))};

  new (data) cxxet::impl::event::list::meta_info{cxxet::impl::get_thread_id(),
                                                 capacity};

  return data;
}

void free_raw_elems(char *const elems) noexcept { free(elems); }

} // namespace cxxet_alternative

void cxxet_list_raw_element_competing_new_delete(benchmark::State &state) {
  auto const capacity{static_cast<int>(state.range(0))};

  for (auto _ : state) {
    auto *elems{cxxet_alternative::new_raw_elems(capacity)};
    benchmark::DoNotOptimize(elems);
    cxxet_alternative::delete_raw_elems(elems);
  }

  state.SetBytesProcessed(state.iterations() *
                          helper::get_internal_array_size(capacity));
}
BENCHMARK(cxxet_list_raw_element_competing_new_delete)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(4096)
    ->Arg(32768);

void cxxet_list_raw_element_competing_malloc_free(benchmark::State &state) {
  auto const capacity{static_cast<int>(state.range(0))};

  for (auto _ : state) {
    auto *elems{cxxet_alternative::malloc_raw_elems(capacity)};
    benchmark::DoNotOptimize(elems);
    cxxet_alternative::free_raw_elems(elems);
  }

  state.SetBytesProcessed(state.iterations() *
                          helper::get_internal_array_size(capacity));
}
BENCHMARK(cxxet_list_raw_element_competing_malloc_free)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(4096)
    ->Arg(32768);

void cxxet_list_raw_element_new_delete_fill_events(benchmark::State &state) {
  auto const capacity{static_cast<int>(state.range(0))};

  for (auto _ : state) {
    auto *elems{cxxet::impl::event::list::raw_element::new_elems(capacity)};
    for (int i = 0; i < capacity; ++i) {
      helper::placement_new_evt(&(elems[1 + i].evt.evt.cmpl));
    }
    cxxet::impl::event::list::raw_element::delete_elems(elems);
  }

  state.SetItemsProcessed(state.iterations() * capacity);
  state.SetComplexityN(capacity);
}
BENCHMARK(cxxet_list_raw_element_new_delete_fill_events)
    ->Complexity(benchmark::BigO::oN)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(4096)
    ->Arg(32768);

// TODO (https://github.com/Ruzovej/cxxet/issues/162) this is approximately 33%
// faster then the current implementation -> rework it later!
void cxxet_list_raw_element_competing_new_delete_fill_events(
    benchmark::State &state) {
  auto const capacity{static_cast<int>(state.range(0))};

  for (auto _ : state) {
    auto *elems{cxxet_alternative::new_raw_elems(capacity)};
    for (int i = 0; i < capacity; ++i) {
      helper::placement_new_evt(elems + static_cast<unsigned>(1 + i) *
                                            sizeof(cxxet::impl::event::any));
    }
    cxxet_alternative::delete_raw_elems(elems);
  }

  state.SetItemsProcessed(state.iterations() * capacity);
  state.SetComplexityN(capacity);
}
BENCHMARK(cxxet_list_raw_element_competing_new_delete_fill_events)
    ->Complexity(benchmark::BigO::oN)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(4096)
    ->Arg(32768);

void cxxet_list_raw_element_competing_malloc_free_fill_events(
    benchmark::State &state) {
  auto const capacity{static_cast<int>(state.range(0))};

  for (auto _ : state) {
    auto *elems{cxxet_alternative::malloc_raw_elems(capacity)};
    for (int i = 0; i < capacity; ++i) {
      helper::placement_new_evt(elems + static_cast<unsigned>(1 + i) *
                                            sizeof(cxxet::impl::event::any));
    }
    cxxet_alternative::free_raw_elems(elems);
  }

  state.SetItemsProcessed(state.iterations() * capacity);
  state.SetComplexityN(capacity);
}
BENCHMARK(cxxet_list_raw_element_competing_malloc_free_fill_events)
    ->Complexity(benchmark::BigO::oN)
    ->Arg(8)
    ->Arg(64)
    ->Arg(512)
    ->Arg(4096)
    ->Arg(32768);

} // namespace

#endif
