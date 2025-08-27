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

#include "impl/event/any.hxx"

#include <climits>
#include <cstddef>

#include <algorithm>
#include <type_traits>
#include <variant>

static_assert(CHAR_BIT == 8, "mysterious platform");

namespace cxxet::impl::event {

namespace {

// Are those meaningful tests?!

struct common_without_explicit_padding {
  trace_type const t;
  char const *const desc;
};

static_assert(sizeof(common<>) <= sizeof(common_without_explicit_padding));

struct common_without_type {
  char const *const desc;
  int const flag_4;
  short const flag_2;
  char const flag_1;
}; // even shuffling members doesn't help ...:

static_assert(sizeof(common<>) < sizeof(std::variant<common_without_type>));

} // namespace

// how to automatically keep this list up to date?! Or maybe put it to separate
// & corresponding files?
static_assert(std::is_trivially_destructible_v<duration_begin>);
static_assert(std::is_trivially_destructible_v<duration_end>);
static_assert(std::is_trivially_destructible_v<complete>);
static_assert(std::is_trivially_destructible_v<instant>);
static_assert(std::is_trivially_destructible_v<counter>);
static_assert(std::is_trivially_destructible_v<metadata>);

static_assert(std::is_trivially_destructible_v<any>);

static constexpr std::size_t max_size{
    std::max({sizeof(duration_begin), sizeof(duration_end), sizeof(complete),
              sizeof(instant), sizeof(counter), sizeof(metadata)})};

static constexpr std::size_t min_size{
    std::min({sizeof(duration_begin), sizeof(duration_end), sizeof(complete),
              sizeof(instant), sizeof(counter), sizeof(metadata)})};

static_assert(min_size < max_size); // TODO add explicit padding to every event
// and change this to equality

static_assert(sizeof(any) == max_size,
              "`any` should have same size as the internal union it wraps!");

} // namespace cxxet::impl::event

#ifdef CXXET_WITH_BENCHMARKS

#include <benchmark/benchmark.h>

namespace {

void cxxet_impl_event_any_default_ctor(benchmark::State &state) {
  for (auto _ : state) {
    cxxet::impl::event::any any{};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_default_ctor);

namespace cxxet_alternative {

struct alignas(alignof(cxxet::impl::event::any)) competing_any {
  competing_any() noexcept { type[0] = 42; }

private:
  char type[sizeof(cxxet::impl::event::any)];
};

static_assert(sizeof(competing_any) == sizeof(cxxet::impl::event::any));

} // namespace cxxet_alternative

void cxxet_competing_any_default_ctor(benchmark::State &state) {
  for (auto _ : state) {
    cxxet_alternative::competing_any any{};
    benchmark::DoNotOptimize(&any);
  }
}
// TODO (#156 ...) this is faster by ca. 50% compared with current solution ->
// rework it later!
BENCHMARK(cxxet_competing_any_default_ctor);

// TODO (#156 ...) those are approximately 33-66 % slower than the default ctor
// -> rework it later!
void cxxet_impl_event_any_from_complete(benchmark::State &state) {
  cxxet::impl::event::complete const e{cxxet::output::category_flag{123},
                                       "complete", 1, 2};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_complete);

void cxxet_impl_event_any_from_counter(benchmark::State &state) {
  cxxet::impl::event::counter const e{cxxet::output::category_flag{456},
                                      "counter", 3, 789.654};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_counter);

void cxxet_impl_event_any_from_duration_begin(benchmark::State &state) {
  cxxet::impl::event::duration_begin const e{cxxet::output::category_flag{456},
                                             "duration_begin", 4};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_duration_begin);

void cxxet_impl_event_any_from_duration_end(benchmark::State &state) {
  cxxet::impl::event::duration_end const e{cxxet::output::category_flag{456},
                                           "duration_end", 5};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_duration_end);

void cxxet_impl_event_any_from_instant(benchmark::State &state) {
  cxxet::impl::event::instant const e{cxxet::output::category_flag{456},
                                      "instant", cxxet::scope_t::process, 6};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_instant);

void cxxet_impl_event_any_from_metadata_name(benchmark::State &state) {
  cxxet::impl::event::metadata const e{
      cxxet::output::category_flag{456}, "metadata name",
      cxxet::impl::event::metadata_type::thread_name};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_metadata_name);

void cxxet_impl_event_any_from_metadata_index(benchmark::State &state) {
  cxxet::impl::event::metadata const e{
      cxxet::output::category_flag{456}, 7,
      cxxet::impl::event::metadata_type::thread_sort_index};

  for (auto _ : state) {
    cxxet::impl::event::any any{e};
    benchmark::DoNotOptimize(&any);
  }
}
BENCHMARK(cxxet_impl_event_any_from_metadata_index);

} // namespace

#endif
