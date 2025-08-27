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

#ifdef CXXET_WITH_BENCHMARKS

#include "cxxet/timepoint.hxx"

#include <chrono>

#include <benchmark/benchmark.h>

namespace {

void timepoint_clock_type_CLOCK_MONOTONIC(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(
        cxxet::impl::now(cxxet::impl::clock_type_t::MONOTONIC));
  }
}
BENCHMARK(timepoint_clock_type_CLOCK_MONOTONIC);

void timepoint_clock_type_CLOCK_THREAD_CPUTIME_ID(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(
        cxxet::impl::now(cxxet::impl::clock_type_t::THREAD_CPUTIME_ID));
  }
}
BENCHMARK(timepoint_clock_type_CLOCK_THREAD_CPUTIME_ID);

void timepoint_chosen_implementation_clock_type_CLOCK_MONOTONIC_RAW(
    benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(
        cxxet::impl::now(cxxet::impl::clock_type_t::MONOTONIC_RAW));
  }
}
BENCHMARK(timepoint_chosen_implementation_clock_type_CLOCK_MONOTONIC_RAW);

void used_timepoint_as_int_ns(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(cxxet::impl::as_int_ns(cxxet::impl::now()));
  }
}
// TODO (#156 -> create next issue ...) this is reported as faster than the
// `...::now()` alone! Research it, and potentially remove this "extra" layer
// and use always this approach.
BENCHMARK(used_timepoint_as_int_ns);

namespace cxxet_alternative {

using timepoint_t = std::chrono::steady_clock::time_point;

[[nodiscard]] inline timepoint_t now() noexcept {
  return std::chrono::steady_clock::now();
}

[[nodiscard]] inline long long as_int_ns(timepoint_t const t) noexcept {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             t.time_since_epoch())
      .count();
}

} // namespace cxxet_alternative

void timepoint_chrono_implementation(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(cxxet_alternative::now());
  }
}
BENCHMARK(timepoint_chrono_implementation);

void chrono_timepoint_as_int_ns(benchmark::State &state) {
  for (auto _ : state) {
    benchmark::DoNotOptimize(
        cxxet_alternative::as_int_ns(cxxet_alternative::now()));
  }
}
// TODO (#156 -> create next issue ...) this is reported as fast as
// `cxxet::impl::now()`! Research it, and potentially remove own implementation
// and use always this approach.
BENCHMARK(chrono_timepoint_as_int_ns);

} // namespace

#endif
