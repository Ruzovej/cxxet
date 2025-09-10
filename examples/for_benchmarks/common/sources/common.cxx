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

#include "common.hxx"

#include <memory>

#include "cxxet/basic.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/sink_diversion.hxx"
#include "cxxet/timepoint.hxx"

namespace {

std::unique_ptr<cxxet::file_sink_handle>
    global_file_sink; // so it can be "flushed" manually

} // namespace

#else

// simple hack ...:
#include "../../../../include/public/cxxet/timepoint.hxx"

#endif

namespace {

long long now() noexcept { return cxxet::impl::as_int_ns(cxxet::impl::now()); }

} // namespace

namespace cxxet_bench {

bool tracing_enabled() noexcept {
#ifdef CXXET_ENABLE
  return true;
#else
  return false;
#endif
}

driver::driver(int const argc, char const **argv)
    : num_iters(argc > 1 ? std::atoi(argv[1]) : 100'000),
      marker_after_iter(argc > 2 ? std::atoi(argv[2]) : 1),
      cxxet_reserve_buffer(argc > 3 ? std::atoi(argv[3]) : 10'000),
      num_threads(argc > 4 ? std::atoi(argv[4]) : 4),
      bench_result_filename{
          argc > 5
              ? argv[5]
              : "/tmp/bench_result" +
                    std::string{tracing_enabled() ? "_traced" : ""} + ".json"} {
#ifdef CXXET_ENABLE
  global_file_sink = cxxet::file_sink_handle::make(num_threads > 1);
#endif
}

driver::~driver() { global_flush(); }

long long driver::thread_reserve(int const capacity) const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink->divert_thread_sink_to_this();
#endif
  CXXET_sink_thread_reserve(capacity > 0 ? capacity : cxxet_reserve_buffer);
  return now() - begin;
}

long long driver::set_thread_name(char const *const th_name) const {
  auto const begin{now()};
  CXXET_mark_thread_name(th_name);
  return now() - begin;
}

long long driver::thread_flush() const {
  auto const begin{now()};
  CXXET_sink_thread_flush_now();
  return now() - begin;
}

long long driver::global_flush_target(std::string filename) const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink->set_flush_target(std::move(filename));
#endif
  return now() - begin;
}

long long driver::global_flush() const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink.reset();
#endif
  return now() - begin;
}

void driver::submit_counter_marker(char const *const name,
                                   double const value) const {
  CXXET_mark_counter(name, value);
}

} // namespace cxxet_bench
