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

#include "cxxet_bench_driver.hxx"

#include <cassert>

#include <fstream>
#include <memory>
#include <mutex>
#include <vector>

#include <nlohmann/json.hpp>

#include "cxxet/basic.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/get_process_id.hxx"
#include "cxxet/get_thread_id.hxx"
#include "cxxet/sink_diversion.hxx"
#include "cxxet/timepoint.hxx"

namespace {

// hack so it can be "flushed" manually:
std::unique_ptr<cxxet::file_sink_handle> global_file_sink{nullptr};

} // namespace

#else

// simple & ugly hacks ...:
#include "../../../../include/public/cxxet/get_process_id.hxx"
#include "../../../../include/public/cxxet/get_thread_id.hxx"
#include "../../../../include/public/cxxet/timepoint.hxx"

#endif

namespace {

long long now() noexcept { return cxxet::impl::as_int_ns(cxxet::impl::now()); }

struct meta {
  long long thread_reserve_ns{0};
  long long set_thread_name_ns{0};
  long long markers_submission_ns{0};
  long long thread_flush_ns{0};
  long long global_flush_target_ns{0};
  long long global_flush_ns{0};

  long long const tid{cxxet::impl::get_thread_id()};

  ~meta();

private:
  bool flush{true};
};

[[nodiscard]] meta &get_meta() noexcept {
  thread_local meta m;
  return m;
}

struct metas {
  metas() = default;
  ~metas();

  void set_traits(std::string aBenchmark_name, int const aNum_iters,
                  int const aMarker_after_iter, int const aCxxet_reserve_buffer,
                  int const aNum_threads, std::string aCxxet_results_filename,
                  std::string aMeta_results_filename) {
    benchmark_name = std::move(aBenchmark_name);
    num_iters = aNum_iters;
    marker_after_iter = aMarker_after_iter;
    cxxet_reserve_buffer = aCxxet_reserve_buffer;
    num_threads = aNum_threads;
    cxxet_results_filename = std::move(aCxxet_results_filename);
    meta_results_filename = std::move(aMeta_results_filename);
  }

  void submit(meta m) {
    std::lock_guard lck{mtx};
    ms.emplace_back(std::move(m));
  }

private:
  metas(metas const &) = delete;
  metas(metas &&) = delete;

  std::mutex mtx;
  std::vector<meta> ms;

  std::string benchmark_name;
  int num_iters;
  int marker_after_iter;
  int cxxet_reserve_buffer;
  int num_threads;
  std::string cxxet_results_filename;
  std::string meta_results_filename;
};

[[nodiscard]] metas &get_metas() {
  static metas m{};
  return m;
}

meta::~meta() {
  if (flush) {
    flush = false;
    get_metas().submit(*this);
  }
}

metas::~metas() {
  assert(!meta_results_filename.empty());

  nlohmann::json meta_info = {
      {"pid", cxxet::impl::get_process_id()},
      {"benchmark_executable", benchmark_name},
      {"benchmark_name",
       benchmark_name.substr(benchmark_name.find_last_of('/') + 1)},
      {"traced", cxxet_bench::driver::tracing_enabled() ? "cxxet" : "bare"},
      {"num_iters", num_iters},
      {"marker_after_iter", marker_after_iter},
      {"cxxet_reserve_buffer", cxxet_reserve_buffer},
      {"num_threads", num_threads},
      {"cxxet_results_filename", cxxet_results_filename},
  };

  nlohmann::json thread_perfs = nlohmann::json::array();
  for (auto const &m : ms) {
    thread_perfs.push_back(
        {{"tid", m.tid},
         {"thread_reserve_ns", m.thread_reserve_ns},
         {"set_thread_name_ns", m.set_thread_name_ns},
         {"markers_submission_ns", m.markers_submission_ns},
         {"thread_flush_ns", m.thread_flush_ns},
         {"global_flush_target_ns", m.global_flush_target_ns},
         {"global_flush_ns", m.global_flush_ns}});
  }

  nlohmann::json result = {
      {"meta_info", std::move(meta_info)},
      {"thread_perfs", std::move(thread_perfs)},
  };

  std::ofstream ofs{meta_results_filename};
  ofs << result.dump(2);
}

} // namespace

namespace cxxet_bench {

driver::driver(int const argc, char const **argv)
    : num_iters(argc > 1 ? std::atoi(argv[1]) : 100'000),
      marker_after_iter(argc > 2 ? std::atoi(argv[2]) : 1),
      cxxet_reserve_buffer(argc > 3 ? std::atoi(argv[3]) : 10'000),
      num_threads(argc > 4 ? std::atoi(argv[4]) : 4),
      bench_result_filename_base{
          (argc > 5 ? argv[5] : "/tmp/bench_result") +
          std::string{tracing_enabled() ? "_traced" : ""}} {
  get_metas().set_traits(argv[0], num_iters, marker_after_iter,
                         cxxet_reserve_buffer, num_threads,
                         bench_result_filename_base + ".json",
                         bench_result_filename_base + "_meta.json");
#ifdef CXXET_ENABLE
  global_file_sink = cxxet::file_sink_handle::make(num_threads > 1);
#endif
}

driver::~driver() {
#ifdef CXXET_ENABLE
  if (global_file_sink == nullptr) {
    return;
  }
#endif
  global_flush();
}

bool driver::tracing_enabled() noexcept {
#ifdef CXXET_ENABLE
  return true;
#else
  return false;
#endif
}

void driver::thread_reserve(int const capacity) const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink->divert_thread_sink_to_this();
#else
  (void)capacity;
#endif
  CXXET_sink_thread_reserve(capacity > 0 ? capacity : cxxet_reserve_buffer);
  get_meta().thread_reserve_ns += now() - begin;
}

void driver::set_thread_name(char const *const th_name) const {
  auto const begin{now()};
  CXXET_mark_thread_name(th_name);
#ifndef CXXET_ENABLE
  (void)th_name;
#endif
  get_meta().set_thread_name_ns += now() - begin;
}

void driver::thread_flush() const {
  auto const begin{now()};
  CXXET_sink_thread_flush_now();
  get_meta().thread_flush_ns += now() - begin;
}

void driver::global_flush_target() const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink->set_flush_target(bench_result_filename_base + ".json");
#endif
  get_meta().global_flush_target_ns += now() - begin;
}

void driver::global_flush() const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink.reset();
#endif
  get_meta().global_flush_ns += now() - begin;
}

void driver::start_marker_submission_measurement() const {
  get_meta().markers_submission_ns -= now();
}

void driver::stop_marker_submission_measurement() const {
  get_meta().markers_submission_ns += now();
}

void driver::submit_counter_marker(char const *const name,
                                   double const value) const {
  CXXET_mark_counter(name, value);
#ifndef CXXET_ENABLE
  (void)name;
  (void)value;
#endif
}

void driver::submit_instant_marker(char const *const name) const {
  CXXET_mark_instant(name);
#ifndef CXXET_ENABLE
  (void)name;
#endif
}

driver::complete_marker_alike::~complete_marker_alike() noexcept {
#ifdef CXXET_ENABLE
  reinterpret_cast<cxxet::mark::complete *>(&buffer)->~complete();
#else
  (void)buffer;
#endif
}

driver::complete_marker_alike::complete_marker_alike(
    const char *const name) noexcept {
#ifdef CXXET_ENABLE
  new (&buffer) cxxet::mark::complete{name};
#else
  (void)name;
#endif
}

[[nodiscard]] driver::complete_marker_alike
driver::submit_complete_marker(char const *const name) const {
  return complete_marker_alike{name};
}

void driver::submit_begin_marker(char const *const name) const {
  CXXET_mark_duration_begin(name);
#ifndef CXXET_ENABLE
  (void)name;
#endif
}

void driver::submit_end_marker() const { CXXET_mark_duration_end(); }

} // namespace cxxet_bench
