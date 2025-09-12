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

std::unique_ptr<cxxet::file_sink_handle>
    global_file_sink; // so it can be "flushed" manually

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
  long long thread_flush_ns{0};
  long long global_flush_target_ns{0};
  long long global_flush_ns{0};

  long long const tid{cxxet::impl::get_thread_id()};

  ~meta();

private:
  bool flush{true};
};

meta &get_meta() noexcept {
  thread_local meta m;
  return m;
}

struct metas {
  explicit metas(std::string aTarget_filename)
      : target_filename{std::move(aTarget_filename)} {
    assert(!target_filename.empty());
  }
  ~metas();

  void submit(meta m) {
    std::lock_guard lck{mtx};
    ms.push_back(std::move(m));
  }

private:
  metas(metas const &) = delete;
  metas(metas &&) = delete;

  std::mutex mtx;
  std::vector<meta> ms;
  std::string target_filename;
};

metas &get_metas(std::string const &target_filename) {
  static metas m{target_filename};
  return m;
}

meta::~meta() {
  if (flush) {
    flush = false;
    get_metas("").submit(*this);
  }
}

metas::~metas() {
  auto const pid{cxxet::impl::get_process_id()};

  nlohmann::json j = nlohmann::json::array();

  for (auto const &m : ms) {
    j.push_back({{"pid", pid},
                 {"tid", m.tid},
                 {"thread_reserve_ns", m.thread_reserve_ns},
                 {"set_thread_name_ns", m.set_thread_name_ns},
                 {"thread_flush_ns", m.thread_flush_ns},
                 {"global_flush_target_ns", m.global_flush_target_ns},
                 {"global_flush_ns", m.global_flush_ns}});
  }

  std::ofstream ofs{target_filename};
  ofs << j.dump(2);
}

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
      bench_result_filename_base{
          (argc > 5 ? argv[5] : "/tmp/bench_result") +
          std::string{tracing_enabled() ? "_traced" : ""}} {
  [[maybe_unused]] auto const &metas{
      get_metas(bench_result_filename_base + "_meta.json")};
#ifdef CXXET_ENABLE
  global_file_sink = cxxet::file_sink_handle::make(num_threads > 1);
#endif
}

driver::~driver() { global_flush(); }

void driver::thread_reserve(int const capacity) const {
  auto const begin{now()};
#ifdef CXXET_ENABLE
  global_file_sink->divert_thread_sink_to_this();
#endif
  CXXET_sink_thread_reserve(capacity > 0 ? capacity : cxxet_reserve_buffer);
  get_meta().thread_reserve_ns += now() - begin;
}

void driver::set_thread_name(char const *const th_name) const {
  auto const begin{now()};
  CXXET_mark_thread_name(th_name);
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

void driver::submit_counter_marker(char const *const name,
                                   double const value) const {
  CXXET_mark_counter(name, value);
}

void driver::submit_instant_marker(char const *const name) const {
  CXXET_mark_instant(name);
}

driver::complete_marker_alike::~complete_marker_alike() noexcept {
#ifdef CXXET_ENABLE
  reinterpret_cast<cxxet::mark::complete *>(&buffer)->~complete();
#endif
}

driver::complete_marker_alike::complete_marker_alike(
    const char *const name) noexcept {
#ifdef CXXET_ENABLE
  new (&buffer) cxxet::mark::complete{name};
#endif
}

[[nodiscard]] driver::complete_marker_alike
driver::submit_complete_marker(char const *const name) const {
  return complete_marker_alike{name};
}

void driver::submit_begin_marker(char const *const name) const {
  CXXET_mark_duration_begin(name);
}

void driver::submit_end_marker() const { CXXET_mark_duration_end(); }

} // namespace cxxet_bench
